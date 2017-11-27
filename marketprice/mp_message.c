/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/16, create
 */

# include <curl/curl.h>

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
    list_t *deals_json;
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
static nw_timer clear_timer;
static nw_timer redis_timer;

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

static void dict_kline_val_free(void *val)
{
    kline_info_free(val);
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

static void list_deals_json_free(void *val)
{
    json_decref(val);
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

static int load_market_deals(redisContext *context, sds key, struct market_info *info)
{
    redisReply *reply = redisCmd(context, "LRANGE %s 0 %d", key, MARKET_DEALS_MAX - 1);
    if (reply == NULL) {
        return -__LINE__;
    }
    for (size_t i = 0; i < reply->elements; ++i) {
        json_t *deal = json_loadb(reply->element[i]->str, reply->element[i]->len, 0, NULL);
        if (deal == NULL) {
            freeReplyObject(reply);
            return -__LINE__;
        }
        list_add_node_tail(info->deals_json, deal);
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

    sdsclear(key);
    key = sdscatprintf(key, "k:%s:deals", info->name);
    ret = load_market_deals(context, key, info);
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
    dt.val_destructor = dict_kline_val_free;

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

    memset(&lt, 0, sizeof(lt));
    lt.free = list_deals_json_free;
    info->deals_json = list_create(&lt);
    if (info->deals_json == NULL)
        return NULL;

    sds key = sdsnew(market);
    dict_add(dict_market, key, info);

    return info;
}

static size_t post_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    sds *reply = userdata;
    *reply = sdscatlen(*reply, ptr, size * nmemb);
    return size * nmemb;
}

static json_t *send_market_list_req(void)
{
    json_t *reply  = NULL;
    json_t *error  = NULL;
    json_t *result = NULL;

    json_t *request = json_object();
    json_object_set_new(request, "method", json_string("market.list"));
    json_object_set_new(request, "params", json_array());
    json_object_set_new(request, "id", json_integer(time(NULL)));
    char *request_data = json_dumps(request, 0);
    json_decref(request);

    CURL *curl = curl_easy_init();
    sds reply_str = sdsempty();

    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_URL, settings.accesshttp);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, post_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &reply_str);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, (long)(1000));
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data);

    CURLcode ret = curl_easy_perform(curl);
    if (ret != CURLE_OK) {
        log_fatal("curl_easy_perform fail: %s", curl_easy_strerror(ret));
        goto cleanup;
    }

    reply = json_loads(reply_str, 0, NULL);
    if (reply == NULL)
        goto cleanup;
    error = json_object_get(reply, "error");
    if (!json_is_null(error)) {
        log_error("get market list fail: %s", reply_str);
        goto cleanup;
    }
    result = json_object_get(reply, "result");
    json_incref(result);

cleanup:
    free(request_data);
    sdsfree(reply_str);
    curl_easy_cleanup(curl);
    curl_slist_free_all(chunk);
    if (reply)
        json_decref(reply);

    return result;
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
    json_t *r = send_market_list_req();
    if (r == NULL) {
        log_error("get market list fail");
        redisFree(context);
        return -__LINE__;
    }
    for (size_t i = 0; i < json_array_size(r); ++i) {
        json_t *item = json_array_get(r, i);
        const char *name = json_string_value(json_object_get(item, "name"));
        log_stderr("init market %s", name);
        struct market_info *info = create_market(name);
        if (info == NULL) {
            log_error("create market %s fail", name);
            json_decref(r);
            redisFree(context);
            return -__LINE__;
        }
        int ret = load_market(context, info);
        if (ret < 0) {
            log_error("load market %s fail: %d", name, ret);
            json_decref(r);
            redisFree(context);
            return -__LINE__;
        }
    }
    json_decref(r);
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

static time_t get_day_start(time_t timestamp)
{
    struct tm *timeinfo = localtime(&timestamp);
    struct tm dtm;
    memset(&dtm, 0, sizeof(dtm));
    dtm.tm_year = timeinfo->tm_year;
    dtm.tm_mon  = timeinfo->tm_mon;
    dtm.tm_mday = timeinfo->tm_mday;
    return mktime(&dtm);
}

static int market_update(const char *market, double timestamp, mpd_t *price, mpd_t *amount, int side, uint64_t id)
{
    struct market_info *info = market_query(market);
    if (info == NULL) {
        info = create_market(market);
        if (info == NULL) {
            return -__LINE__;
        }
    }

    // update sec
    time_t time_sec = (time_t)timestamp;
    dict_entry *entry;
    struct kline_info *kinfo = NULL;
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
    time_t time_day = get_day_start(time_sec);
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
    if (side == MARKET_ORDER_SIDE_ASK) {
        json_object_set_new(deal, "type", json_string("sell"));
    } else {
        json_object_set_new(deal, "type", json_string("buy"));
    }

    list_add_node_tail(info->deals, json_dumps(deal, 0));
    list_add_node_head(info->deals_json, deal);
    if (info->deals_json->len > MARKET_DEALS_MAX) {
        list_del(info->deals_json, list_tail(info->deals_json));
    }

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
    if (side != MARKET_ORDER_SIDE_ASK && side != MARKET_ORDER_SIDE_BID) {
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

static int flush_deals(redisContext *context, const char *market, list_t *list)
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

    reply = redisCmd(context, "LTRIM k:%s:deals 0 %d", market, MARKET_DEALS_MAX - 1);
    if (reply) {
        freeReplyObject(reply);
    }

    list_clear(list);
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
            log_fatal("flush_kline fail: %d, type: %d, timestamp: %ld", ret, key->kline_type, key->timestamp);
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
        if (info->deals->len == 0)
            continue;
        ret = flush_deals(context, info->name, info->deals);
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
        log_fatal("flush_market fail: %d", ret);
    }
}

static void on_clear_timer(nw_timer *timer, void *privdata)
{
    clear_kline();
}

static int clear_key(redisContext *context, const char *key, time_t end)
{
    redisReply *reply = redisCmd(context, "HGETALL %s", key);
    if (reply == NULL)
        return -__LINE__;
    for (size_t i = 0; i < reply->elements; i += 2) {
        time_t timestamp = strtol(reply->element[i]->str, NULL, 0);
        if (timestamp >= end)
            continue;
        redisReply *r = redisCmd(context, "HDEL %s %ld", key, timestamp);
        if (r == NULL)
            return -__LINE__;
        freeReplyObject(r);
    }
    freeReplyObject(reply);

    return 0;
}

static int clear_redis(void)
{
    redisContext *context = redis_sentinel_connect_master(redis);
    if (context == NULL)
        return 1;
    time_t now = time(NULL);
    dict_iterator *iter = dict_get_iterator(dict_market);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct market_info *info = entry->val;
        sds key = sdsempty();
        key = sdscatprintf(key, "k:%s:1s", info->name);
        clear_key(context, key, now - settings.sec_max);
        sdsclear(key);

        key = sdscatprintf(key, "k:%s:1m", info->name);
        clear_key(context, key, now / 60 * 60 - settings.min_max * 60);
        sdsclear(key);

        key = sdscatprintf(key, "k:%s:1h", info->name);
        clear_key(context, key, now / 3600 * 3600 - settings.hour_max * 3600);
        sdsfree(key);
    }
    redisFree(context);

    return 0;
}

static void on_redis_timer(nw_timer *timer, void *privdata)
{
    int pid = fork();
    if (pid == 0) {
        _exit(clear_redis());
    }
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

    nw_timer_set(&clear_timer, 3600, true, on_clear_timer, NULL);
    nw_timer_start(&clear_timer);

    nw_timer_set(&redis_timer, 86400, true, on_redis_timer, NULL);
    nw_timer_start(&redis_timer);

    return 0;
}

bool market_exist(const char *market)
{
    struct market_info *info = market_query(market);
    if (info)
        return true;
    return false;
}

static struct kline_info *get_last_kline(dict_t *dict, time_t start, time_t end, int interval)
{
    for (; start >= end; start -= interval) {
        dict_entry *entry = dict_find(dict, &start);
        if (entry) {
            return entry->val;
        }
    }

    return NULL;
}

json_t *get_market_status(const char *market, int period)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    struct kline_info *kinfo = NULL;
    time_t now = time(NULL);
    time_t start = now - period;
    time_t start_min = start / 60 * 60 + 60;

    for (time_t timestamp = start; timestamp < start_min; timestamp++) {
        dict_entry *entry = dict_find(info->sec, &timestamp);
        if (!entry)
            continue;
        struct kline_info *sinfo = entry->val;
        if (kinfo == NULL) {
            kinfo = kline_info_new(sinfo->open);
        }
        kline_info_merge(kinfo, sinfo);
    }

    for (time_t timestamp = start_min; timestamp < now; timestamp += 60) {
        dict_entry *entry = dict_find(info->min, &timestamp);
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
    json_object_set_new_mpd(result, "deal", kinfo->deal);

    kline_info_free(kinfo);

    return result;
}

json_t *get_market_status_today(const char *market)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    json_t *result = json_object();
    time_t now = time(NULL);
    time_t start = get_day_start(now);
    struct kline_info *klast = get_last_kline(info->day, start - 86400, start - 86400 * 30, 86400);
    dict_entry *entry = dict_find(info->day, &start);
    if (entry) {
        struct kline_info *today = entry->val;
        json_object_set_new_mpd(result, "open", today->open);
        json_object_set_new_mpd(result, "last", today->close);
        json_object_set_new_mpd(result, "high", today->high);
        json_object_set_new_mpd(result, "low",  today->low);
    } else if (klast) {
        json_object_set_new_mpd(result, "open", klast->close);
        json_object_set_new_mpd(result, "last", klast->close);
        json_object_set_new_mpd(result, "high", klast->close);
        json_object_set_new_mpd(result, "low",  klast->close);
    } else {
        json_object_set_new(result, "open", json_string("0"));
        json_object_set_new(result, "last", json_string("0"));
        json_object_set_new(result, "high", json_string("0"));
        json_object_set_new(result, "low",  json_string("0"));
    }

    mpd_t *volume = mpd_qncopy(mpd_zero);
    mpd_t *deal = mpd_qncopy(mpd_zero);
    time_t start_24h = now - 86400;
    time_t start_min = start_24h / 60 * 60 + 60;

    for (time_t timestamp = start_24h; timestamp < start_min; timestamp++) {
        dict_entry *entry = dict_find(info->sec, &timestamp);
        if (!entry)
            continue;
        struct kline_info *info = entry->val;
        mpd_add(volume, volume, info->volume, &mpd_ctx);
        mpd_add(deal, deal, info->deal, &mpd_ctx);
    }

    for (time_t timestamp = start_min; timestamp < now; timestamp += 60) {
        dict_entry *entry = dict_find(info->min, &timestamp);
        if (!entry)
            continue;
        struct kline_info *info = entry->val;
        mpd_add(volume, volume, info->volume, &mpd_ctx);
        mpd_add(deal, deal, info->deal, &mpd_ctx);
    }

    json_object_set_new_mpd(result, "volume", volume);
    json_object_set_new_mpd(result, "deal", deal);
    mpd_del(volume);
    mpd_del(deal);

    return result;
}

static int append_kinfo(json_t *result, time_t timestamp, struct kline_info *kinfo, const char *market)
{
    json_t *unit = json_array();
    json_array_append_new(unit, json_integer(timestamp));
    json_array_append_new_mpd(unit, kinfo->open);
    json_array_append_new_mpd(unit, kinfo->close);
    json_array_append_new_mpd(unit, kinfo->high);
    json_array_append_new_mpd(unit, kinfo->low);
    json_array_append_new_mpd(unit, kinfo->volume);
    json_array_append_new_mpd(unit, kinfo->deal);
    json_array_append_new(unit, json_string(market));
    json_array_append_new(result, unit);

    return 0;
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
    start = start / interval * interval;
    struct kline_info *kbefor = get_last_kline(info->sec, start - 1, now - settings.sec_max, 1);
    struct kline_info *klast = kbefor;
    for (; start <= end; start += interval) {
        struct kline_info *kinfo = NULL;
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
        append_kinfo(result, start, kinfo, market);
        if (klast && klast != kbefor)
            kline_info_free(klast);
        klast = kinfo;
    }
    if (klast && klast != kbefor)
        kline_info_free(klast);

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
    struct kline_info *kbefor = get_last_kline(info->min, start - 60, start_min, 60);
    struct kline_info *klast = kbefor;
    int step = interval / 60;
    for (; start <= end; start += interval) {
        struct kline_info *kinfo = NULL;
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
        append_kinfo(result, start, kinfo, market);
        if (klast && klast != kbefor)
            kline_info_free(klast);
        klast = kinfo;
    }
    if (klast && klast != kbefor)
        kline_info_free(klast);

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
    time_t base = get_day_start(start);
    while ((base + interval) <= start)
        base += interval;
    start = base;

    struct kline_info *kbefor = get_last_kline(info->hour, start - 3600, start_min, 3600);
    struct kline_info *klast = kbefor;
    int step = interval / 3600;
    for (; start <= end; start += interval) {
        struct kline_info *kinfo = NULL;
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
        append_kinfo(result, start, kinfo, market);
        if (klast && klast != kbefor)
            kline_info_free(klast);
        klast = kinfo;
    }
    if (klast && klast != kbefor)
        kline_info_free(klast);

    return result;
}

json_t *get_market_kline_day(const char *market, time_t start, time_t end, int interval)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    json_t *result = json_array();
    start = (start - settings.timezone) / interval * interval + settings.timezone;

    struct kline_info *kbefor = get_last_kline(info->day, start - 86400, start - 86400 * 30, 86400);
    struct kline_info *klast = kbefor;
    int step = interval / 86400;
    for (; start <= end; start += interval) {
        struct kline_info *kinfo = NULL;
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
        append_kinfo(result, start, kinfo, market);
        if (klast && klast != kbefor)
            kline_info_free(klast);
        klast = kinfo;
    }
    if (klast && klast != kbefor)
        kline_info_free(klast);

    return result;
}

json_t *get_market_kline_week(const char *market, time_t start, time_t end, int interval)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    json_t *result = json_array();
    time_t base = (start - settings.timezone) / interval * interval - 3 * 86400 + settings.timezone;
    while ((base + interval) <= start)
        base += interval;
    start = base;

    struct kline_info *kbefor = get_last_kline(info->day, start - 86400, start - 86400 * 30, 86400);
    struct kline_info *klast = kbefor;
    int step = interval / 86400;
    for (; start <= end; start += interval) {
        struct kline_info *kinfo = NULL;
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
        append_kinfo(result, start, kinfo, market);
        if (klast && klast != kbefor)
            kline_info_free(klast);
        klast = kinfo;
    }
    if (klast && klast != kbefor)
        kline_info_free(klast);

    return result;
}

static time_t get_month_start(int tm_year, int tm_mon)
{
    struct tm mtm;
    memset(&mtm, 0, sizeof(mtm));
    mtm.tm_year = tm_year;
    mtm.tm_mon  = tm_mon;
    mtm.tm_mday = 1;
    return mktime(&mtm);
}

static time_t get_next_month(int *tm_year, int *tm_mon)
{
    if (*tm_mon == 11) {
        *tm_mon = 0;
        *tm_year += 1;
    } else {
        *tm_mon += 1;
    }
    struct tm mtm;
    memset(&mtm, 0, sizeof(mtm));
    mtm.tm_year = *tm_year;
    mtm.tm_mon  = *tm_mon;
    mtm.tm_mday = 1;
    return mktime(&mtm);
}

json_t *get_market_kline_month(const char *market, time_t start, time_t end, int interval)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    json_t *result = json_array();
    struct tm *timeinfo = localtime(&start);
    int tm_year = timeinfo->tm_year;
    int tm_mon  = timeinfo->tm_mon;
    time_t mon_start = get_month_start(tm_year, tm_mon);

    struct kline_info *kbefor = get_last_kline(info->day, mon_start - 86400, start - 86400 * 30, 86400);
    struct kline_info *klast = kbefor;
    for (; mon_start <= end; ) {
        struct kline_info *kinfo = NULL;
        time_t mon_next = get_next_month(&tm_year, &tm_mon);
        time_t timestamp = mon_start;
        for (; timestamp < mon_next && timestamp <= end; timestamp += 86400) {
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
        append_kinfo(result, mon_start, kinfo, market);
        mon_start = mon_next;
        if (klast && klast != kbefor)
            kline_info_free(klast);
        klast = kinfo;
    }
    if (klast && klast != kbefor)
        kline_info_free(klast);

    return result;
}

json_t *get_market_deals(const char *market, int limit, uint64_t last_id)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    int count = 0;
    json_t *result = json_array();
    list_iter *iter = list_get_iterator(info->deals_json, LIST_START_HEAD);
    list_node *node;
    while ((node = list_next(iter)) != NULL) {
        json_t *deal = node->value;
        uint64_t id = json_integer_value(json_object_get(deal, "id"));
        if (id <= last_id) {
            break;
        }
        json_array_append(result, deal);
        count += 1;
        if (count == limit) {
            break;
        }
    }
    list_release_iterator(iter);

    return result;
}

mpd_t  *get_market_last_price(const char *market)
{
    struct market_info *info = market_query(market);
    if (info == NULL)
        return NULL;

    return info->last;
}

