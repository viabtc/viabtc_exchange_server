/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/16, create
 */

# include "mp_config.h"
# include "mp_message.h"
# include "mp_kline.h"

struct market_info {
    char   *name;
    mpd_t  *last;
    dict_t *sec;
    dict_t *min;
    list_t *deals;
};

static redis_sentinel_t *redis;
static kafka_consumer_t *deals;
static dict_t *dict_market;

static double   last_flush;
static int64_t  last_offset;
static nw_timer timer;

static uint32_t dict_sds_key_hash_func(const void *key)
{
    return dict_generic_hash_function(key, sdslen((sds)key));
}

static int dict_sds_key_compare(const void *key1, const void *key2)
{
    return sdscmp((sds)key1, (sds)key2);
}

static void dict_sds_key_free(void *key)
{
    sdsfree(key);
}

static uint32_t dict_time_key_hash_func(const void *key)
{
    return dict_generic_hash_function(key, sizeof(time_t));
}

static int dict_time_key_compare(const void *key1, const void *key2)
{
    return *(time_t *)key1 - *(time_t *)key2;
}

static void *dict_time_key_dup(const void *key)
{
    time_t *obj = malloc(sizeof(time_t));
    *obj = *(time_t *)key;
    return obj;
}

static void dict_time_key_free(void *key)
{
    free(key);
}

static void list_deals_free(void *val)
{
    free(val);
}

static int load_market_sec(redisContext *context, struct market_info *market)
{
    redisReply *reply = redisCmd(context, "HGETALL k:%s:1s", market->name);
    if (reply == NULL) {
        return -__LINE__;
    }
    for (size_t i = 0; i < reply->elements; i += 2) {
        time_t timestamp = strtol(reply->element[i]->str, NULL, 0);
        struct kline_info *info = kline_from_str(reply->element[i + 1]->str);
        if (timestamp && info) {
            dict_add(market->sec, &timestamp, info);
        }
    }
    freeReplyObject(reply);

    return 0;
}

static int init_market_min(struct market_info *market)
{
    dict_iterator *iter = dict_get_iterator(market->sec);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        time_t timestamp = *(time_t *)entry->key;
        time_t base = timestamp / 60 * 60;
        struct kline_info *sec_info = entry->val;
        struct kline_info *min_info = NULL;
        dict_entry *result = dict_find(market->min, &base);
        if (result) {
            min_info = result->val;
        } else {
            min_info = kline_info_new(sec_info->open);
            dict_add(market->min, &base, min_info);
        }
        kline_info_merge(min_info, sec_info);
    }
    dict_release_iterator(iter);

    return 0;
}

static int load_market_last(redisContext *context, struct market_info *market)
{
    redisReply *reply = redisCmd(context, "GET k:%s:last", market->name);
    if (reply == NULL) {
        return -__LINE__;
    }
    if (reply->type == REDIS_REPLY_STRING) {
        market->last = decimal(reply->str, 0);
        if (market->last == NULL) {
            freeReplyObject(reply);
            return -__LINE__;
        }
    }
    freeReplyObject(reply);

    return 0;
}

static int load_market_kline(redisContext *context, struct market_info *market)
{
    int ret;
    ret = load_market_sec(context, market);
    if (ret < 0) {
        return ret;
    }
    ret = load_market_last(context, market);
    if (ret < 0) {
        return ret;
    }
    ret = init_market_min(market);
    if (ret < 0) {
        return ret;
    }

    return 0;
}

static int64_t get_message_offset(void)
{
    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL)
        return -__LINE__;
    redisReply *reply = redisCmd(context, "GET k:offset");
    if (reply == NULL) {
        redisFree(context);
        return -__LINE__;
    }
    int64_t offset = 0;
    if (reply->type == REDIS_REPLY_STRING) {
        offset = strtoll(reply->str, NULL, 0);
    }
    freeReplyObject(reply);
    redisFree(context);

    return offset;
}

static int add_markets_list(const char *market)
{
    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL)
        return -__LINE__;
    redisReply *reply = redisCmd(context, "SADD k:markets %s", market);
    if (reply == NULL) {
        redisFree(context);
        return -__LINE__;
    }
    freeReplyObject(reply);
    redisFree(context);

    return 0;
}

static struct market_info *create_market(const char *market)
{
    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function = dict_time_key_hash_func;
    dt.key_compare = dict_time_key_compare;
    dt.key_dup = dict_time_key_dup;
    dt.key_destructor = dict_time_key_free;

    list_type lt;
    memset(&lt, 0, sizeof(lt));
    lt.free = list_deals_free;

    struct market_info *info = malloc(sizeof(struct market_info));
    memset(info, 0, sizeof(struct market_info));
    info->name = strdup(market);
    info->last = mpd_qncopy(mpd_zero);
    info->sec = dict_create(&dt, 1024);
    info->min = dict_create(&dt, 1024);
    info->deals = list_create(&lt);
    if (info->sec == NULL || info->min == NULL || info->deals == NULL)
        return NULL;

    sds key = sdsnew(market);
    dict_add(dict_market, key, info);

    return info;
}

static int init_market(void)
{
    dict_types type;
    memset(&type, 0, sizeof(type));
    type.hash_function = dict_sds_key_hash_func;
    type.key_compare = dict_sds_key_compare;
    type.key_destructor = dict_sds_key_free;
    dict_market = dict_create(&type, 64);
    if (dict_market == NULL)
        return -__LINE__;

    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL)
        return -__LINE__;
    redisReply *reply = redisCmd(context, "SMEMBERS k:markets");
    if (reply == NULL) {
        redisFree(context);
        return -__LINE__;
    }

    for (size_t i = 0; i < reply->elements; ++i) {
        struct market_info *info = create_market(reply->element[i]->str);
        if (info == NULL) {
            freeReplyObject(reply);
            redisFree(context);
            return -__LINE__;
        }
        int ret = load_market_kline(context, info);
        if (ret < 0) {
            freeReplyObject(reply);
            redisFree(context);
            return ret;
        }
    }
    freeReplyObject(reply);
    redisFree(context);

    last_flush = current_timestamp();

    return 0;
}

static struct market_info *market_query(const char *market)
{
    sds key = sdsnew(market);
    dict_entry *entry = dict_find(dict_market, key);
    if (entry) {
        sdsfree(key);
        return entry->val;
    }
    sdsfree(key);
    return NULL;
}

static int flush_kline_sec(redisContext *context, const char *market, time_t timestamp, struct kline_info *info)
{
    char *str = kline_to_str(info);
    if (str == NULL)
        return -__LINE__;
    redisReply *reply = redisCmd(context, "HSET k:%s:1s %ld %s", market, timestamp, str);
    if (reply == NULL) {
        free(str);
        return -__LINE__;
    }
    free(str);
    freeReplyObject(reply);

    return 0;
}

static int flush_kline_min(redisContext *context, const char *market, time_t timestamp, struct kline_info *info)
{
    char *str = kline_to_str(info);
    if (str == NULL)
        return -__LINE__;
    redisReply *reply = redisCmd(context, "HSET k:%s:1m %ld %s", market, timestamp, str);
    if (reply == NULL) {
        free(str);
        return -__LINE__;
    }
    free(str);
    freeReplyObject(reply);

    return 0;
}

static int flush_offset(redisContext *context, int64_t offset)
{
    redisReply *reply = redisCmd(context, "SET k:offset %"PRIi64, offset);
    if (reply == NULL) {
        return -__LINE__;
    }
    freeReplyObject(reply);

    return 0;
}

static int flush_last(redisContext *context, const char *market, mpd_t *last)
{
    char *last_str = mpd_to_sci(last, 0);
    if (last_str == NULL)
        return -__LINE__;
    redisReply *reply = redisCmd(context, "SET k:%s:last %s", market, last_str);
    if (reply == NULL) {
        free(last_str);
        return -__LINE__;
    }
    free(last_str);
    freeReplyObject(reply);

    return 0;
}

static int market_update(const char *market, double timestamp, mpd_t *price, mpd_t *amount, int side, uint64_t id)
{
    struct market_info *info = market_query(market);
    if (info == NULL) {
        info = create_market(market);
        if (info == NULL) {
            return -__LINE__;
        }
        int ret = add_markets_list(market);
        if (ret < 0) {
            log_fatal("add market: %s fail: %d", market, ret);
            return -__LINE__;
        }
    }

    // update sec
    time_t time_sec = (time_t)timestamp;
    dict_entry *entry;
    struct kline_info *kinfo;
    entry = dict_find(info->sec, &time_sec);
    if (entry) {
        kinfo = entry->val;
    } else {
        kinfo = kline_info_new(price);
        if (kinfo == NULL)
            return -__LINE__;
        dict_add(info->sec, &time_sec, kinfo);
    }
    kline_info_update(kinfo, price, amount);

    // update min
    time_t base = time_sec / 60 * 60;
    entry = dict_find(info->min, &base);
    if (entry) {
        kinfo = entry->val;
    } else {
        kinfo = kline_info_new(price);
        if (kinfo == NULL)
            return -__LINE__;
        dict_add(info->min, &base, kinfo);
    }
    kline_info_update(kinfo, price, amount);

    // update last
    mpd_copy(info->last, price, &mpd_ctx);

    // append deals
    json_t *deal = json_object();
    json_object_set_new(deal, "id", json_integer(id));
    json_object_set_new(deal, "time", json_real(timestamp));
    json_object_set_new_mpd(deal, "price", price);
    json_object_set_new_mpd(deal, "amount", amount);
    if (side == MARKET_SIDE_SELL) {
        json_object_set_new(deal, "type", json_string("sell"));
    } else {
        json_object_set_new(deal, "type", json_string("buy"));
    }

    list_add_node_tail(info->deals, json_dumps(deal, 0));
    json_decref(deal);

    return 0;
}

static void on_deals_message(sds message, int64_t offset)
{
    log_trace("deals message: %s, offset: %"PRIi64, message, offset);
    json_t *obj = json_loadb(message, sdslen(message), 0, NULL);
    if (obj == NULL) {
        log_error("invalid message: %s, offset: %"PRIi64, message, offset);
        return;
    }

    mpd_t *price = NULL;
    mpd_t *amount = NULL;

    if (!json_is_array(obj) || json_array_size(obj) < 12) {
        goto cleanup;
    }
    double timestamp = json_real_value(json_array_get(obj, 0));
    if (timestamp == 0) {
        goto cleanup;
    }
    const char *market = json_string_value(json_array_get(obj, 1));
    if (!market) {
        goto cleanup;
    }
    const char *price_str = json_string_value(json_array_get(obj, 6));
    if (!price_str || (price = decimal(price_str, 0)) == NULL) {
        goto cleanup;
    }
    const char *amount_str = json_string_value(json_array_get(obj, 7));
    if (!amount_str || (amount = decimal(amount_str, 0)) == NULL) {
        goto cleanup;
    }
    int side = json_integer_value(json_array_get(obj, 10));
    if (side != MARKET_SIDE_SELL && side != MARKET_SIDE_BUY) {
        goto cleanup;
    }
    uint64_t id = json_integer_value(json_array_get(obj, 11));
    if (id == 0) {
        goto cleanup;
    }

    int ret = market_update(market, timestamp, price, amount, side, id);
    if (ret < 0) {
        log_error("market_update fail %d, message: %s", ret, message);
        goto cleanup;
    }
    last_offset = offset;

    mpd_del(price);
    mpd_del(amount);
    json_decref(obj);
    return;

cleanup:
    log_error("invalid message: %s, offset: %"PRIi64, message, offset);
    if (price)
        mpd_del(price);
    if (amount)
        mpd_del(amount);
    json_decref(obj);
}

static int flush_sec_dict(redisContext *context, const char *market, dict_t *dict, time_t expire)
{
    dict_iterator *iter = dict_get_iterator(dict);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        time_t timestamp = *(time_t *)entry->key;
        struct kline_info *info = entry->val;
        if (info->update > last_flush) {
            int ret = flush_kline_sec(context, market, timestamp, info);
            if (ret < 0) {
                log_error("flush_kline_sec %ld fail: %d", timestamp, ret);
                dict_release_iterator(iter);
                return -__LINE__;
            }
        }
        if (timestamp < expire) {
            dict_delete(dict, entry->key);
        }
    }
    dict_release_iterator(iter);

    return 0;
}

static int flush_min_dict(redisContext *context, const char *market, dict_t *dict, time_t expire)
{
    dict_iterator *iter = dict_get_iterator(dict);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        time_t timestamp = *(time_t *)entry->key;
        struct kline_info *info = entry->val;
        if (info->update > last_flush) {
            int ret = flush_kline_min(context, market, timestamp, info);
            if (ret < 0) {
                log_error("flush_kline_min %ld fail: %d", timestamp, ret);
                dict_release_iterator(iter);
                return -__LINE__;
            }
        }
        if (timestamp < expire) {
            dict_delete(dict, entry->key);
        }
    }
    dict_release_iterator(iter);

    return 0;
}

static int flush_deals(redisContext *context, const char *market, list_t *list)
{
    if (list->len == 0)
        return 0;

    int argc = 2 + list->len;
    const char **argv = malloc(sizeof(char *) * argc);
    size_t *argvlen = malloc(sizeof(size_t) * argc);

    argv[0] = "LPUSH";
    argvlen[0] = strlen(argv[0]);

    sds key = sdsempty();
    key = sdscatprintf(key, "k:%s:deals", market);
    argv[1] = key;
    argvlen[1] = sdslen(key);

    list_iter *iter = list_get_iterator(list, LIST_START_HEAD);
    list_node *node;
    size_t index = 0;
    while ((node = list_next(iter)) != NULL) {
        argv[2 + index] = node->value;
        argvlen[2 + index] = strlen((char *)node->value);
        index += 1;
    }
    list_release_iterator(iter);

    log_debug("LPUSH");
    redisReply *reply = redisCommandArgv(context, argc, argv, argvlen);
    if (reply == NULL) {
        sdsfree(key);
        free(argv);
        free(argvlen);
        return -__LINE__;
    }
    sdsfree(key);
    free(argv);
    free(argvlen);
    freeReplyObject(reply);

    reply = redisCmd(context, "LTRIM m:%s:deals 0 9999", market);
    if (reply) {
        freeReplyObject(reply);
    }

    list_clear(list);
    return 0;
}

static int flush_market(void)
{
    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL)
        return -__LINE__;

    int ret;
    time_t expire = time(NULL) / 60 * 60 - 86400;
    dict_iterator *iter = dict_get_iterator(dict_market);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct market_info *info = entry->val;
        ret = flush_sec_dict(context, info->name, info->sec, expire);
        if (ret < 0) {
            redisFree(context);
            dict_release_iterator(iter);
            return -__LINE__;
        }
        ret = flush_min_dict(context, info->name, info->min, expire);
        if (ret < 0) {
            redisFree(context);
            dict_release_iterator(iter);
            return -__LINE__;
        }
        ret = flush_last(context, info->name, info->last);
        if (ret < 0) {
            redisFree(context);
            dict_release_iterator(iter);
            return -__LINE__;
        }
        ret = flush_deals(context, info->name, info->deals);
        if (ret < 0) {
            redisFree(context);
            dict_release_iterator(iter);
            return -__LINE__;
        }
    }
    dict_release_iterator(iter);

    ret = flush_offset(context, last_offset);
    if (ret < 0) {
        redisFree(context);
        return -__LINE__;
    }

    redisFree(context);
    return 0;
}

static void on_timer(nw_timer *timer, void *privdata)
{
    int ret = flush_market();
    if (ret < 0) {
        log_error("flush_market fail: %d", ret);
    }
}

int init_message(void)
{
    int ret;
    redis = redis_sentinel_create(&settings.redis);
    if (redis == NULL)
        return -__LINE__;
    ret = init_market();
    if (ret < 0) {
        return ret;
    }
    last_offset = get_message_offset();
    if (last_offset < 0) {
        return -__LINE__;
    }
    settings.deals.offset = last_offset + 1;
    deals = kafka_consumer_create(&settings.deals, on_deals_message);
    if (deals == NULL) {
        return -__LINE__;
    }

    nw_timer_set(&timer, 1, true, on_timer, NULL);
    nw_timer_start(&timer);

    return 0;
}

json_t *get_market_status(const char *market, int period)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    struct kline_info *kinfo = NULL;
    time_t start = time(NULL) - period;
    for (int i = 0; i < period; ++i) {
        time_t timestamp = start + i;
        dict_entry *entry = dict_find(info->sec, &timestamp);
        if (!entry)
            continue;
        struct kline_info *sinfo = entry->val;
        if (kinfo == NULL) {
            kinfo = kline_info_new(sinfo->open);
        }
        kline_info_merge(kinfo, sinfo);
    }

    if (kinfo == NULL)
        kinfo = kline_info_new(mpd_zero);

    json_t *result = json_object();
    json_object_set_new(result, "period", json_integer(period));
    json_object_set_new_mpd(result, "last", info->last);
    json_object_set_new_mpd(result, "open", kinfo->open);
    json_object_set_new_mpd(result, "close", kinfo->close);
    json_object_set_new_mpd(result, "high", kinfo->high);
    json_object_set_new_mpd(result, "low", kinfo->low);
    json_object_set_new_mpd(result, "volume", kinfo->volume);

    kline_info_free(kinfo);

    return result;
}

json_t *get_market_deals(const char *market, int limit)
{
    return NULL;
}

