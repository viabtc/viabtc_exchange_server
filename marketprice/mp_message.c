/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/16, create
 */

# include "mp_config.h"
# include "mp_message.h"

struct kline_info {
    mpd_t *open;
    mpd_t *close;
    mpd_t *high;
    mpd_t *low;
    mpd_t *volume;
    int64_t offset;
};

struct market_info {
    char  *name;
    time_t sec_update;
    struct kline_info *sec;
    time_t min_update;
    struct kline_info *min;
    dict_t *dict_sec;
    dict_t *dict_min;
};

static redis_sentinel_t *redis;
static kafka_consumer_t *deals;
static dict_t *dict_market;

static time_t flush_sec_error;
static time_t flush_min_error;

static struct kline_info *kline_info_new(mpd_t *open)
{
    struct kline_info *info = malloc(sizeof(struct kline_info));
    if (info == NULL)
        return NULL;
    info->open  = mpd_qncopy(open);
    info->close = mpd_qncopy(open);
    info->high  = mpd_qncopy(open);
    info->low   = mpd_qncopy(open);
    info->volume = mpd_qncopy(mpd_zero);
    info->offset = 0;

    return info;
}

static void kline_info_update(struct kline_info *info, mpd_t *price, mpd_t *amount, int64_t offset)
{
    mpd_copy(info->close, price, &mpd_ctx);
    mpd_add(info->volume, info->volume, amount, &mpd_ctx);
    if (mpd_cmp(price, info->high, &mpd_ctx) > 0)
        mpd_copy(info->high, price, &mpd_ctx);
    if (mpd_cmp(price, info->low, &mpd_ctx) < 0)
        mpd_copy(info->low, price, &mpd_ctx);
    if (offset > info->offset)
        info->offset = offset;
}

static void kline_info_merge(struct kline_info *info, struct kline_info *update)
{
    mpd_copy(info->close, update->close, &mpd_ctx);
    mpd_add(info->volume, info->volume, update->volume, &mpd_ctx);
    if (mpd_cmp(update->high, info->high, &mpd_ctx) > 0)
        mpd_copy(info->high, update->high, &mpd_ctx);
    if (mpd_cmp(update->low, info->low, &mpd_ctx) < 0)
        mpd_copy(info->low, update->low, &mpd_ctx);
    info->offset = update->offset;
}

static void kline_info_free(struct kline_info *info)
{
    if (info->open)
        mpd_del(info->open);
    if (info->close)
        mpd_del(info->close);
    if (info->high)
        mpd_del(info->high);
    if (info->low)
        mpd_del(info->low);
    if (info->volume)
        mpd_del(info->volume);
    free(info);
}

static struct kline_info *kline_from_str(char *str)
{
    json_t *obj = json_loadb(str, strlen(str), 0, NULL);
    if (obj == NULL) {
        return NULL;
    }
    if (!json_is_array(obj) || json_array_size(obj) != 5) {
        json_decref(obj);
        return NULL;
    }
    struct kline_info *info = malloc(sizeof(struct kline_info));
    if (info == NULL) {
        json_decref(obj);
        return NULL;
    }

    memset(info, 0, sizeof(struct kline_info));
    const char *open = json_string_value(json_array_get(obj, 0));
    if (!open || (info->open = decimal(open, 0)) == NULL) {
        goto cleanup;
    }
    const char *close = json_string_value(json_array_get(obj, 0));
    if (!close || (info->close = decimal(close, 1)) == NULL) {
        goto cleanup;
    }
    const char *high = json_string_value(json_array_get(obj, 0));
    if (!high || (info->high = decimal(high, 2)) == NULL) {
        goto cleanup;
    }
    const char *low = json_string_value(json_array_get(obj, 0));
    if (!low || (info->low = decimal(low, 3)) == NULL) {
        goto cleanup;
    }
    const char *volume = json_string_value(json_array_get(obj, 0));
    if (!volume || (info->volume = decimal(volume, 4)) == NULL) {
        goto cleanup;
    }

    json_decref(obj);
    return info;

cleanup:
    json_decref(obj);
    kline_info_free(info);
    return NULL;
}

static char *kline_to_str(struct kline_info *info)
{
    json_t *obj = json_array();
    json_array_append_new_mpd(obj, info->open);
    json_array_append_new_mpd(obj, info->close);
    json_array_append_new_mpd(obj, info->high);
    json_array_append_new_mpd(obj, info->low);
    json_array_append_new_mpd(obj, info->volume);
    char *str = json_dumps(obj, 0);
    json_decref(obj);
    return str;
}

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

static int load_market_sec(redisContext *context, struct market_info *market)
{
    time_t start = time(NULL) / 60 * 60 - 86400;
    time_t end = start + 86400;
    sds cmd = sdsnew("HMGET k:%s:1s");
    for (time_t timestamp = start; timestamp < end; timestamp++) {
        cmd = sdscatprintf(cmd, " %ld", timestamp);
    }
    redisReply *reply = redisRawCmd(context, cmd);
    if (reply == NULL) {
        log_error("redis cmd: %s fail", cmd);
        sdsfree(cmd);
        return -__LINE__;
    }
    sdsfree(cmd);

    for (size_t i = 0; i < reply->elements; ++i) {
        if (reply->element[i]->type != REDIS_REPLY_STRING)
            continue;
        time_t timestamp = start + i;
        struct kline_info *info = kline_from_str(reply->element[i]->str);
        if (info == NULL) {
            log_error("invalid kline info, timestamp: %ld, str: %s", timestamp, reply->element[i]->str);
            freeReplyObject(reply);
            return -__LINE__;
        }
        dict_add(market->dict_sec, &timestamp, info);
    }

    freeReplyObject(reply);
    return 0;
}

static int load_market_min(redisContext *context, struct market_info *market)
{
    time_t start = time(NULL) / 60 * 60 - 86400;
    time_t end = start + 86400;
    for (time_t timestamp = start; timestamp < end; timestamp++) {
        dict_entry *entry = dict_find(market->dict_sec, &timestamp);
        if (!entry) {
            continue;
        }
        struct kline_info *info = entry->val;
        if (timestamp / 60 > market->min_update / 60) {
            if (market->min_update && market->min) {
                time_t start = market->min_update / 60 * 60;
                dict_add(market->dict_min, &start, market->min);
            }
            market->min = kline_info_new(info->open);
        }
        market->min_update = timestamp;
        kline_info_merge(market->min, info);
    }

    return 0;
}

static int load_market_kline(struct market_info *market)
{
    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL)
        return -__LINE__;
    int ret;
    ret = load_market_sec(context, market);
    if (ret < 0) {
        redisFree(context);
        return ret;
    }
    ret = load_market_min(context, market);
    if (ret < 0) {
        redisFree(context);
        return ret;
    }
    redisFree(context);
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

static sds *get_markets_list(size_t *count)
{
    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL)
        return NULL;
    redisReply *reply = redisCmd(context, "SMEMBERS k:markets");
    if (reply == NULL) {
        redisFree(context);
        return NULL;
    }
    *count = reply->elements;
    sds *markets = malloc(sizeof(sds) * reply->elements);
    for (size_t i = 0; i < reply->elements; ++i) {
        markets[i] = sdsnewlen(reply->element[i]->str, reply->element[i]->len);
    }
    freeReplyObject(reply);
    redisFree(context);
    return markets;
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
    info->dict_sec = dict_create(&type, 1024);
    info->dict_min = dict_create(&type, 1024);
    if (info->dict_sec == NULL || info->dict_min == NULL)
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

    size_t count;
    sds *markets = get_markets_list(&count);
    if (markets == NULL)
        return -__LINE__;

    for (size_t i = 0; i < count; ++i) {
        struct market_info *info = create_market(markets[i]);
        if (info == NULL)
            return -__LINE__;
        int ret = load_market_kline(info);
        if (ret < 0)
            return ret;
    }

    return 0;
}

static struct market_info *market_query(const char *market)
{
    sds key = sdsnew(market);
    dict_entry *entry = dict_find(dict_market, key);
    struct market_info *info = NULL;
    if (entry) {
        info = entry->val;
    }
    sdsfree(key);
    return info;
}

static int flush_kline_sec(const char *market, time_t timestamp, struct kline_info *info)
{
    char *str = kline_to_str(info);
    if (str == NULL)
        return -__LINE__;
    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL) {
        free(str);
        return -__LINE__;
    }

    redisReply *reply = redisCmd(context, "HSET k:%s:1s %ld %s", market, timestamp, str);
    if (reply == NULL) {
        free(str);
        redisFree(context);
        return -__LINE__;
    }
    free(str);
    freeReplyObject(reply);

    reply = redisCmd(context, "SET k:offset %"PRIi64, info->offset);
    if (reply == NULL) {
        redisFree(context);
        return -__LINE__;
    }
    freeReplyObject(reply);

    redisFree(context);
    return 0;
}

static int flush_kline_min(const char *market, time_t timestamp, struct kline_info *info)
{
    char *str = kline_to_str(info);
    if (str == NULL)
        return -__LINE__;
    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL) {
        free(str);
        return -__LINE__;
    }
    redisReply *reply = redisCmd(context, "HSET k:%s:1m %ld %s", market, timestamp, str);
    if (reply == NULL) {
        free(str);
        redisFree(context);
        return -__LINE__;
    }
    free(str);
    freeReplyObject(reply);
    redisFree(context);
    return 0;
}

static int market_update(const char *market, time_t timestamp, mpd_t *price, mpd_t *amount, int64_t offset)
{
    struct market_info *info = market_query(market);
    if (info == NULL) {
        info = create_market(market);
        if (info == NULL) {
            return -__LINE__;
        }
        int ret = add_markets_list(market);
        if (ret < 0) {
            log_error("add market: %s fail: %d", market, ret);
            return -__LINE__;
        }
    }

    if (timestamp != info->sec_update) {
        if (info->sec_update && info->sec) {
            dict_add(info->dict_sec, &info->sec_update, info->sec);
            int ret = flush_kline_sec(info->name, info->sec_update, info->sec);
            if (ret < 0) {
                log_error("flush_kline_sec: %ld fail: %d", info->sec_update, ret);
                flush_sec_error = info->sec_update;
            }
        }
        info->sec = kline_info_new(price);
    }
    info->sec_update = timestamp;
    kline_info_update(info->sec, price, amount, offset);

    if (timestamp / 60 != info->min_update / 60) {
        if (info->min_update && info->min) {
            time_t start = info->min_update / 60 * 60;
            dict_add(info->dict_min, &start, info->min);
            int ret = flush_kline_min(info->name, start, info->min);
            if (ret < 0) {
                log_error("flush_kline_min: %ld fail: %d", start, ret);
                flush_min_error = start;
            }
        }
        info->min = kline_info_new(price);
    }
    info->min_update = timestamp;
    kline_info_update(info->min, price, amount, offset);

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
    int ret = market_update(market, (time_t)timestamp, price, amount, offset);
    if (ret < 0) {
        goto cleanup;
    }

cleanup:
    log_error("invalid message: %s, offset: %"PRIi64, message, offset);
    if (price)
        mpd_del(price);
    if (amount)
        mpd_del(amount);
    json_decref(obj);
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
    int64_t offset = get_message_offset();
    if (offset < 0) {
        return offset;
    }
    settings.deals.offset = offset;
    deals = kafka_consumer_create(&settings.deals, on_deals_message);
    if (deals == NULL) {
        return -__LINE__;
    }

    return 0;
}

