/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/16, create
 */

# include "mp_config.h"
# include "mp_message.h"
# include "mp_kline.h"

struct market_info {
    char   *name;
    dict_t *sec;
    dict_t *min;
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

static int load_market_sec(struct market_info *market)
{
    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL)
        return -__LINE__;
    redisReply *reply = redisCmd(context, "HGETALL k:%s:1s", market->name);
    if (reply == NULL) {
        redisFree(context);
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
    redisFree(context);

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

static int load_market_kline(struct market_info *market)
{
    int ret;
    ret = load_market_sec(market);
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
    dict_types type;
    memset(&type, 0, sizeof(type));
    type.hash_function = dict_time_key_hash_func;
    type.key_compare = dict_time_key_compare;
    type.key_dup = dict_time_key_dup;
    type.key_destructor = dict_time_key_free;

    struct market_info *info = malloc(sizeof(struct market_info));
    memset(info, 0, sizeof(struct market_info));
    info->name = strdup(market);
    info->sec = dict_create(&type, 1024);
    info->min = dict_create(&type, 1024);
    if (info->sec == NULL || info->min == NULL)
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
    redisFree(context);

    for (size_t i = 0; i < reply->elements; ++i) {
        struct market_info *info = create_market(reply->element[i]->str);
        if (info == NULL) {
            freeReplyObject(reply);
            return -__LINE__;
        }
        int ret = load_market_kline(info);
        if (ret < 0) {
            freeReplyObject(reply);
            return ret;
        }
    }
    freeReplyObject(reply);
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

static int market_update(const char *market, time_t timestamp, mpd_t *price, mpd_t *amount)
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

    dict_entry *entry;
    struct kline_info *kinfo;
    entry = dict_find(info->sec, &timestamp);
    if (entry) {
        kinfo = entry->val;
    } else {
        kinfo = kline_info_new(price);
        if (kinfo == NULL)
            return -__LINE__;
        dict_add(info->sec, &timestamp, kinfo);
    }
    kline_info_update(kinfo, price, amount);

    time_t base = timestamp / 60 * 60;
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

    if (!json_is_array(obj) || json_array_size(obj) != 10) {
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
    int ret = market_update(market, (time_t)timestamp, price, amount);
    if (ret < 0) {
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

static int clear_kline_dict(dict_t *dict, time_t start)
{
    dict_iterator *iter = dict_get_iterator(dict);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        if (*(time_t *)entry->key < start) {
            dict_delete(dict, entry->key);
        }
    }
    dict_release_iterator(iter);

    return 0;
}

static int clear_kline(void)
{
    time_t start = time(NULL) / 60 * 60 - 86400;
    dict_iterator *iter = dict_get_iterator(dict_market);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct market_info *info = entry->val;
        clear_kline_dict(info->sec, start);
        clear_kline_dict(info->min, start);
    }
    dict_release_iterator(iter);

    return 0;
}

static int flush_sec_dict(redisContext *context, const char *market, dict_t *dict)
{
    dict_iterator *iter = dict_get_iterator(dict);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        time_t timestamp = *(time_t *)entry->key;
        struct kline_info *info = entry->val;
        if (info->update < last_flush)
            continue;
        int ret = flush_kline_sec(context, market, timestamp, info);
        if (ret < 0) {
            log_error("flush_kline_sec %ld fail: %d", timestamp, ret);
            dict_release_iterator(iter);
            return -__LINE__;
        }
    }
    dict_release_iterator(iter);

    return 0;
}

static int flush_min_dict(redisContext *context, const char *market, dict_t *dict)
{
    dict_iterator *iter = dict_get_iterator(dict);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        time_t timestamp = *(time_t *)entry->key;
        struct kline_info *info = entry->val;
        if (info->update < last_flush)
            continue;
        int ret = flush_kline_min(context, market, timestamp, info);
        if (ret < 0) {
            log_error("flush_kline_min %ld fail: %d", timestamp, ret);
            dict_release_iterator(iter);
            return -__LINE__;
        }
    }
    dict_release_iterator(iter);

    return 0;
}

static int flush_kline(void)
{
    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL)
        return -__LINE__;

    int ret;
    dict_iterator *iter = dict_get_iterator(dict_market);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct market_info *info = entry->val;
        ret = flush_sec_dict(context, info->name, info->sec);
        if (ret < 0) {
            redisFree(context);
            dict_release_iterator(iter);
            return -__LINE__;
        }
        ret = flush_min_dict(context, info->name, info->min);
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
    int ret;
    ret = flush_kline();
    if (ret < 0) {
        log_error("flush_kline fail: %d", ret);
    }
    ret = clear_kline();
    if (ret < 0) {
        log_error("clear_kline fail: %d", ret);
    }
}

int init_message(void)
{
    redis = redis_sentinel_create(&settings.redis);
    if (redis == NULL)
        return -__LINE__;
    int ret = init_market();
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

