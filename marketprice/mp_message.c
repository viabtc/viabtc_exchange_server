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
    dict_t *hour;
    dict_t *day;
    dict_t *update;
    list_t *deals;
    double update_time;
};

enum {
    KLINE_SEC,
    KLINE_MIN,
    KLINE_HOUR,
    KLINE_DAY,
};

struct update_key {
    int kline_type;
    time_t timestamp;
};

static redis_sentinel_t *redis;
static kafka_consumer_t *deals;
static dict_t *dict_market;

static double   last_flush;
static int64_t  last_offset;
static nw_timer market_timer;
static nw_timer deals_timer;
static nw_timer clear_timer;

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

static uint32_t dict_update_key_hash_func(const void *key)
{
    return dict_generic_hash_function(key, sizeof(struct update_key));
}

static int dict_update_key_compare(const void *key1, const void *key2)
{
    return memcmp(key1, key2, sizeof(struct update_key));
}

static void *dict_update_key_dup(const void *key)
{
    struct update_key *obj = malloc(sizeof(struct update_key));
    memcpy(obj, key, sizeof(struct update_key));
    return obj;
}

static void dict_update_key_free(void *key)
{
    free(key);
}

static void list_deals_free(void *val)
{
    free(val);
}

static int load_market_kline(redisContext *context, sds key, dict_t *dict, time_t start)
{
    redisReply *reply = redisCmd(context, "HGETALL %s", key);
    if (reply == NULL) {
        return -__LINE__;
    }
    for (size_t i = 0; i < reply->elements; i += 2) {
        time_t timestamp = strtol(reply->element[i]->str, NULL, 0);
        if (start && timestamp < start)
            continue;
        struct kline_info *info = kline_from_str(reply->element[i + 1]->str);
        if (info) {
            dict_add(dict, &timestamp, info);
        }
    }
    freeReplyObject(reply);

    return 0;
}

static int load_market_last(redisContext *context, struct market_info *info)
{
    redisReply *reply = redisCmd(context, "GET k:%s:last", info->name);
    if (reply == NULL) {
        return -__LINE__;
    }
    if (reply->type == REDIS_REPLY_STRING) {
        info->last = decimal(reply->str, 0);
        if (info->last == NULL) {
            freeReplyObject(reply);
            return -__LINE__;
        }
    }
    freeReplyObject(reply);

    return 0;
}

static int load_market(redisContext *context, struct market_info *info)
{
    time_t now = time(NULL);
    int ret;

    sds key = sdsempty();
    key = sdscatprintf(key, "k:%s:1s", info->name);
    ret = load_market_kline(context, key, info->sec, now - settings.sec_max);
    if (ret < 0) {
        sdsfree(key);
        return ret;
    }

    sdsclear(key);
    key = sdscatprintf(key, "k:%s:1m", info->name);
    ret = load_market_kline(context, key, info->min, now / 60 * 60 - settings.min_max * 60);
    if (ret < 0) {
        sdsfree(key);
        return ret;
    }

    sdsclear(key);
    key = sdscatprintf(key, "k:%s:1h", info->name);
    ret = load_market_kline(context, key, info->hour, now / 3600 * 3600 - settings.hour_max * 3600);
    if (ret < 0) {
        sdsfree(key);
        return ret;
    }

    sdsclear(key);
    key = sdscatprintf(key, "k:%s:1d", info->name);
    ret = load_market_kline(context, key, info->day, 0);
    if (ret < 0) {
        sdsfree(key);
        return ret;
    }
    sdsfree(key);

    ret = load_market_last(context, info);
    if (ret < 0) {
        return ret;
    }

    return 0;
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
    struct market_info *info = malloc(sizeof(struct market_info));
    memset(info, 0, sizeof(struct market_info));
    info->name = strdup(market);
    info->last = mpd_qncopy(mpd_zero);
    dict_types dt;

    memset(&dt, 0, sizeof(dt));
    dt.hash_function = dict_time_key_hash_func;
    dt.key_compare = dict_time_key_compare;
    dt.key_dup = dict_time_key_dup;
    dt.key_destructor = dict_time_key_free;

    info->sec = dict_create(&dt, 1024);
    info->min = dict_create(&dt, 1024);
    info->hour = dict_create(&dt, 1024);
    info->day = dict_create(&dt, 1024);
    if (info->sec == NULL || info->min == NULL || info->hour == NULL || info->day == NULL)
        return NULL;

    memset(&dt, 0, sizeof(dt));
    dt.hash_function = dict_update_key_hash_func;
    dt.key_compare = dict_update_key_compare;
    dt.key_dup = dict_update_key_dup;
    dt.key_destructor = dict_update_key_free;
    info->update = dict_create(&dt, 1024);
    if (info->update == NULL)
        return NULL;

    list_type lt;
    memset(&lt, 0, sizeof(lt));
    lt.free = list_deals_free;
    info->deals = list_create(&lt);
    if (info->deals == NULL)
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
        int ret = load_market(context, info);
        if (ret < 0) {
            freeReplyObject(reply);
            redisFree(context);
            return ret;
        }
    }
    freeReplyObject(reply);
    redisFree(context);

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

static struct kline_info *kline_query(dict_t *dict, time_t timestamp)
{
    dict_entry *entry = dict_find(dict, &timestamp);
    if (entry == NULL)
        return NULL;
    return entry->val;
}

static void add_update(struct market_info *info, int type, time_t timestamp)
{
    struct update_key key;
    key.kline_type = type;
    key.timestamp = timestamp;
    dict_add(info->update, &key, NULL);
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
    add_update(info, KLINE_SEC, time_sec);

    // update min
    time_t time_min = time_sec / 60 * 60;
    entry = dict_find(info->min, &time_min);
    if (entry) {
        kinfo = entry->val;
    } else {
        kinfo = kline_info_new(price);
        if (kinfo == NULL)
            return -__LINE__;
        dict_add(info->min, &time_min, kinfo);
    }
    kline_info_update(kinfo, price, amount);
    add_update(info, KLINE_MIN, time_min);

    // update hour
    time_t time_hour = time_sec / 3600 * 3600;
    entry = dict_find(info->hour, &time_hour);
    if (entry) {
        kinfo = entry->val;
    } else {
        kinfo = kline_info_new(price);
        if (kinfo == NULL)
            return -__LINE__;
        dict_add(info->hour, &time_hour, kinfo);
    }
    kline_info_update(kinfo, price, amount);
    add_update(info, KLINE_HOUR, time_hour);

    // update day
    time_t time_day = time_sec / 86400 * 86400 + settings.timezone;
    entry = dict_find(info->day, &time_day);
    if (entry) {
        kinfo = entry->val;
    } else {
        kinfo = kline_info_new(price);
        if (kinfo == NULL)
            return -__LINE__;
        dict_add(info->day, &time_day, kinfo);
    }
    kline_info_update(kinfo, price, amount);
    add_update(info, KLINE_DAY, time_day);

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

    // update time
    info->update_time = current_timestamp();

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

static int flush_deals_list(redisContext *context, const char *market, list_t *list)
{
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

    reply = redisCmd(context, "LTRIM m:%s:deals 0 %d", market, MARKET_DEALS_MAX - 1);
    if (reply) {
        freeReplyObject(reply);
    }

    list_clear(list);
    return 0;
}

static int flush_deals(void)
{
    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL)
        return -__LINE__;

    int ret;
    dict_iterator *iter = dict_get_iterator(dict_market);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct market_info *info = entry->val;
        if (info->deals->len == 0)
            continue;
        ret = flush_deals_list(context, info->name, info->deals);
        if (ret < 0) {
            redisFree(context);
            dict_release_iterator(iter);
            return ret;
        }
    }
    dict_release_iterator(iter);

    redisFree(context);
    return 0;
}

static int flush_kline(redisContext *context, struct market_info *info, struct update_key *ukey)
{
    sds key = sdsempty();
    struct kline_info *kinfo = NULL;
    if (ukey->kline_type == KLINE_SEC) {
        key = sdscatprintf(key, "k:%s:1s", info->name);
        kinfo = kline_query(info->sec, ukey->timestamp);
    } else if (ukey->kline_type == KLINE_MIN) {
        key = sdscatprintf(key, "k:%s:1m", info->name);
        kinfo = kline_query(info->min, ukey->timestamp);
    } else if (ukey->kline_type == KLINE_HOUR) {
        key = sdscatprintf(key, "k:%s:1h", info->name);
        kinfo = kline_query(info->hour, ukey->timestamp);
    } else {
        key = sdscatprintf(key, "k:%s:1d", info->name);
        kinfo = kline_query(info->day, ukey->timestamp);
    }
    if (kinfo == NULL) {
        sdsfree(key);
        return -__LINE__;
    }

    char *str = kline_to_str(kinfo);
    if (str == NULL) {
        sdsfree(key);
        return -__LINE__;
    }

    redisReply *reply = redisCmd(context, "HSET %s %ld %s", key, ukey->timestamp, str);
    if (reply == NULL) {
        free(str);
        sdsfree(key);
        return -__LINE__;
    }
    free(str);
    sdsfree(key);
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
static int flush_update(redisContext *context, struct market_info *info)
{
    dict_iterator *iter = dict_get_iterator(info->update);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct update_key *key = entry->key;
        log_trace("flush_kline type: %d, timestamp: %ld", key->kline_type, key->timestamp);
        int ret = flush_kline(context, info, key);
        if (ret < 0) {
            log_error("flush_kline fail: %d, type: %d, timestamp: %ld", ret, key->kline_type, key->timestamp);
        }
        dict_delete(info->update, entry->key);
    }
    dict_release_iterator(iter);

    return 0;
}

static int flush_market(void)
{
    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL)
        return -__LINE__;

    int ret;
    dict_iterator *iter = dict_get_iterator(dict_market);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct market_info *info = entry->val;
        if (info->update_time < last_flush)
            continue;
        ret = flush_update(context, info);
        if (ret < 0) {
            redisFree(context);
            dict_release_iterator(iter);
            return ret;
        }
        ret = flush_last(context, info->name, info->last);
        if (ret < 0) {
            redisFree(context);
            dict_release_iterator(iter);
            return ret;
        }
    }
    dict_release_iterator(iter);

    ret = flush_offset(context, last_offset);
    if (ret < 0) {
        redisFree(context);
        return -__LINE__;
    }

    last_flush = current_timestamp();
    redisFree(context);

    return 0;
}

static void clear_dict(dict_t *dict, time_t start)
{
    dict_iterator *iter = dict_get_iterator(dict);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        time_t timestamp = *(time_t *)entry->key;
        if (timestamp < start) {
            dict_delete(dict, entry->key);
        }
    }
    dict_release_iterator(iter);
}

static void clear_kline(void)
{
    time_t now = time(NULL);
    dict_iterator *iter = dict_get_iterator(dict_market);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct market_info *info = entry->val;
        clear_dict(info->sec, now - settings.sec_max);
        clear_dict(info->sec, now / 60 * 60 - settings.min_max * 60);
        clear_dict(info->hour, now / 3600 * 3600 - settings.hour_max * 3600);
    }
    dict_release_iterator(iter);
}

static void on_market_timer(nw_timer *timer, void *privdata)
{
    int ret = flush_market();
    if (ret < 0) {
        log_error("flush_market fail: %d", ret);
    }
}

static void on_deals_timer(nw_timer *timer, void *privdata)
{
    int ret = flush_deals();
    if (ret < 0) {
        log_error("flush_market fail: %d", ret);
    }
}

static void on_clear_timer(nw_timer *timer, void *privdata)
{
    clear_kline();
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

    nw_timer_set(&market_timer, 10, true, on_market_timer, NULL);
    nw_timer_start(&market_timer);

    nw_timer_set(&deals_timer, 0.1, true, on_deals_timer, NULL);
    nw_timer_start(&deals_timer);

    nw_timer_set(&clear_timer, 3600, true, on_clear_timer, NULL);
    nw_timer_start(&clear_timer);

    return 0;
}

bool market_exist(const char *market)
{
    struct market_info *info = market_query(market);
    if (info)
        return true;
    return false;
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

static int append_kinfo(json_t *result, time_t timestamp, struct kline_info *kinfo)
{
    json_t *unit = json_array();
    json_array_append_new(unit, json_integer(timestamp));
    json_array_append_new_mpd(unit, kinfo->open);
    json_array_append_new_mpd(unit, kinfo->close);
    json_array_append_new_mpd(unit, kinfo->high);
    json_array_append_new_mpd(unit, kinfo->low);
    json_array_append_new_mpd(unit, kinfo->volume);
    json_array_append_new(result, unit);

    return 0;
}

struct kline_info *get_last_kline(dict_t *dict, time_t start, time_t end, int interval)
{
    for (; start >= end; start -= interval) {
        dict_entry *entry = dict_find(dict, &start);
        if (entry) {
            return entry->val;
        }
    }

    return NULL;
}

json_t *get_market_kline_sec(const char *market, time_t start, time_t end, int interval)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    json_t *result = json_array();
    time_t now = time(NULL);
    if (start < now - settings.sec_max)
        start = now - settings.sec_max;
    struct kline_info *klast = get_last_kline(info->sec, start - 1, now - settings.sec_max, 1);
    struct kline_info *kinfo;
    for (; start <= end; start += interval) {
        kinfo = NULL;
        for (int i = 0; i < interval; ++i) {
            time_t timestamp = start + i;
            dict_entry *entry = dict_find(info->sec, &timestamp);
            if (entry == NULL)
                continue;
            struct kline_info *item = entry->val;
            if (kinfo == NULL)
                kinfo = kline_info_new(item->open);
            kline_info_merge(kinfo, item);
        }
        if (kinfo == NULL) {
            if (klast == NULL) {
                continue;
            }
            kinfo = kline_info_new(klast->close);
        }
        append_kinfo(result, start, kinfo);
        klast = kinfo;
    }

    return result;
}

json_t *get_market_kline_min(const char *market, time_t start, time_t end, int interval)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    json_t *result = json_array();
    time_t now = time(NULL);
    time_t start_min = now / 60 * 60 - settings.min_max * 60;
    if (start < start_min)
        start = start_min;
    start = start / interval * interval;
    struct kline_info *klast = get_last_kline(info->min, start - 60, start_min, 60);
    struct kline_info *kinfo;
    int step = interval / 60;
    for (; start <= end; start += interval) {
        kinfo = NULL;
        for (int i = 0; i < step; ++i) {
            time_t timestamp = start + i * 60;
            dict_entry *entry = dict_find(info->min, &timestamp);
            if (entry == NULL)
                continue;
            struct kline_info *item = entry->val;
            if (kinfo == NULL)
                kinfo = kline_info_new(item->open);
            kline_info_merge(kinfo, item);
        }
        if (kinfo == NULL) {
            if (klast == NULL) {
                continue;
            }
            kinfo = kline_info_new(klast->close);
        }
        append_kinfo(result, start, kinfo);
        klast = kinfo;
    }

    return result;
}

json_t *get_market_kline_hour(const char *market, time_t start, time_t end, int interval)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    json_t *result = json_array();
    time_t now = time(NULL);
    time_t start_min = now / 3600 * 3600 - settings.hour_max * 3600;
    if (start < start_min)
        start = start_min;
    time_t base = start / 86400 * 86400 - 86400 + settings.timezone;
    while (base <= start) {
        base += interval;
    }
    start = base - interval;

    struct kline_info *klast = get_last_kline(info->hour, start - 3600, start_min, 3600);
    struct kline_info *kinfo;
    int step = interval / 3600;
    for (; start <= end; start += interval) {
        kinfo = NULL;
        for (int i = 0; i < step; ++i) {
            time_t timestamp = start + i * 3600;
            dict_entry *entry = dict_find(info->hour, &timestamp);
            if (entry == NULL)
                continue;
            struct kline_info *item = entry->val;
            if (kinfo == NULL)
                kinfo = kline_info_new(item->open);
            kline_info_merge(kinfo, item);
        }
        if (kinfo == NULL) {
            if (klast == NULL) {
                continue;
            }
            kinfo = kline_info_new(klast->close);
        }
        append_kinfo(result, start, kinfo);
        klast = kinfo;
    }

    return result;
}

json_t *get_market_kline_day(const char *market, time_t start, time_t end, int interval)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    json_t *result = json_array();
    time_t base = start / interval * interval + settings.timezone;
    if (base > start)
        base -= interval;
    start = base;

    struct kline_info *klast = get_last_kline(info->day, start - 86400, start - 86400 * 30, 86400);
    struct kline_info *kinfo;
    int step = interval / 86400;
    for (; start <= end; start += interval) {
        kinfo = NULL;
        for (int i = 0; i < step; ++i) {
            time_t timestamp = start + i * 86400;
            dict_entry *entry = dict_find(info->day, &timestamp);
            if (entry == NULL)
                continue;
            struct kline_info *item = entry->val;
            if (kinfo == NULL)
                kinfo = kline_info_new(item->open);
            kline_info_merge(kinfo, item);
        }
        if (kinfo == NULL) {
            if (klast == NULL) {
                continue;
            }
            kinfo = kline_info_new(klast->close);
        }
        append_kinfo(result, start, kinfo);
        klast = kinfo;
    }

    return result;
}

json_t *get_market_kline_week(const char *market, time_t start, time_t end, int interval)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    json_t *result = json_array();
    time_t base = start / interval * interval - 3 * 86400 + settings.timezone;
    while (base <= start)
        base += interval;
    start = base - interval;

    struct kline_info *klast = get_last_kline(info->day, start - 86400, start - 86400 * 30, 86400);
    struct kline_info *kinfo;
    int step = interval / 86400;
    for (; start <= end; start += interval) {
        kinfo = NULL;
        for (int i = 0; i < step; ++i) {
            time_t timestamp = start + i * 86400;
            dict_entry *entry = dict_find(info->day, &timestamp);
            if (entry == NULL)
                continue;
            struct kline_info *item = entry->val;
            if (kinfo == NULL)
                kinfo = kline_info_new(item->open);
            kline_info_merge(kinfo, item);
        }
        if (kinfo == NULL) {
            if (klast == NULL) {
                continue;
            }
            kinfo = kline_info_new(klast->close);
        }
        append_kinfo(result, start, kinfo);
        klast = kinfo;
    }

    return result;
}

json_t *get_market_deals(const char *market, int limit)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL)
        return NULL;
    redisReply *reply = redisCmd(context, "LRANGE k:%s:deals 0 %d", market, limit - 1);
    if (reply == NULL) {
        redisFree(context);
        return NULL;
    }
    redisFree(context);

    json_t *result = json_array();
    for (size_t i = 0; i < reply->elements; ++i) {
        json_t *deal = json_loadb(reply->element[i]->str, reply->element[i]->len, 0, NULL);
        if (deal == NULL) {
            log_error("invalid deal info: %s", reply->element[i]->str);
            freeReplyObject(reply);
            json_decref(result);
            return NULL;
        }
        json_array_append_new(result, deal);
    }
    freeReplyObject(reply);

    return result;
}
