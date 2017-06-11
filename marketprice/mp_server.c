/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/18, create
 */

# include "mp_config.h"
# include "mp_server.h"
# include "mp_message.h"

static rpc_svr *svr;
static dict_t *dict_cache;
static nw_timer cache_timer;

struct cache_val {
    double      time;
    json_t      *result;
};

static int reply_json(nw_ses *ses, rpc_pkg *pkg, const json_t *json)
{
    char *message_data;
    if (settings.debug) {
        message_data = json_dumps(json, JSON_INDENT(4));
    } else {
        message_data = json_dumps(json, 0);
    }
    if (message_data == NULL)
        return -__LINE__;
    log_trace("connection: %s send: %s", nw_sock_human_addr(&ses->peer_addr), message_data);

    rpc_pkg reply;
    memcpy(&reply, pkg, sizeof(reply));
    reply.pkg_type = RPC_PKG_TYPE_REPLY;
    reply.body = message_data;
    reply.body_size = strlen(message_data);
    rpc_send(ses, &reply);
    free(message_data);

    return 0;
}

static int reply_error(nw_ses *ses, rpc_pkg *pkg, int code, const char *message)
{
    json_t *error = json_object();
    json_object_set_new(error, "code", json_integer(code));
    json_object_set_new(error, "message", json_string(message));

    json_t *reply = json_object();
    json_object_set_new(reply, "error", error);
    json_object_set_new(reply, "result", json_null());
    json_object_set_new(reply, "id", json_integer(pkg->req_id));

    int ret = reply_json(ses, pkg, reply);
    json_decref(reply);

    return ret;
}

static int reply_error_invalid_argument(nw_ses *ses, rpc_pkg *pkg)
{
    return reply_error(ses, pkg, 1, "invalid argument");
}

static int reply_error_internal_error(nw_ses *ses, rpc_pkg *pkg)
{
    return reply_error(ses, pkg, 2, "internal error");
}

static int reply_result(nw_ses *ses, rpc_pkg *pkg, json_t *result)
{
    json_t *reply = json_object();
    json_object_set_new(reply, "error", json_null());
    json_object_set    (reply, "result", result);
    json_object_set_new(reply, "id", json_integer(pkg->req_id));

    int ret = reply_json(ses, pkg, reply);
    json_decref(reply);

    return ret;
}

static bool process_cache(nw_ses *ses, rpc_pkg *pkg, sds *cache_key)
{
    sds key = sdsempty();
    key = sdscatprintf(key, "%u", pkg->command);
    key = sdscatlen(key, pkg->body, pkg->body_size);
    dict_entry *entry = dict_find(dict_cache, key);
    if (entry == NULL) {
        *cache_key = key;
        return false;
    }

    struct cache_val *cache = entry->val;
    double now = current_timestamp();
    if ((now - cache->time) > settings.cache_timeout) {
        dict_delete(dict_cache, key);
        *cache_key = key;
        return false;
    }

    reply_result(ses, pkg, cache->result);
    sdsfree(key);
    return true;
}

static int add_cache(sds cache_key, json_t *result)
{
    struct cache_val cache;
    cache.time = current_timestamp();
    cache.result = result;
    json_incref(result);
    dict_replace(dict_cache, cache_key, &cache);

    return 0;
}

static int on_cmd_market_status(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 2)
        return reply_error_invalid_argument(ses, pkg);

    const char *market = json_string_value(json_array_get(params, 0));
    if (!market)
        return reply_error_invalid_argument(ses, pkg);
    if (!market_exist(market))
        return reply_error_invalid_argument(ses, pkg);

    int period = json_integer_value(json_array_get(params, 1));
    if (period <= 0 || period > settings.sec_max)
        return reply_error_invalid_argument(ses, pkg);

    sds cache_key = NULL;
    if (process_cache(ses, pkg, &cache_key))
        return 0;

    json_t *result = get_market_status(market, period);
    if (result == NULL) {
        sdsfree(cache_key);
        return reply_error_internal_error(ses, pkg);
    }

    add_cache(cache_key, result);
    sdsfree(cache_key);

    int ret = reply_result(ses, pkg, result);
    json_decref(result);
    return ret;
}

static int on_cmd_market_kline(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 4)
        return reply_error_invalid_argument(ses, pkg);

    const char *market = json_string_value(json_array_get(params, 0));
    if (!market)
        return reply_error_invalid_argument(ses, pkg);
    if (!market_exist(market))
        return reply_error_invalid_argument(ses, pkg);

    time_t start = json_integer_value(json_array_get(params, 1));
    if (start <= 0)
        return reply_error_invalid_argument(ses, pkg);

    time_t end = json_integer_value(json_array_get(params, 2));
    time_t now = time(NULL);
    if (end > now)
        end = now;
    if (end <= 0 || start > end)
        return reply_error_invalid_argument(ses, pkg);

    int interval = json_integer_value(json_array_get(params, 3));
    if (interval <= 0)
        return reply_error_invalid_argument(ses, pkg);

    sds cache_key = NULL;
    if (process_cache(ses, pkg, &cache_key))
        return 0;

    json_t *result = NULL;
    if (interval < 60) {
        if (60 % interval != 0) {
            sdsfree(cache_key);
            return reply_error_invalid_argument(ses, pkg);
        }
        result = get_market_kline_sec(market, start, end, interval);
    } else if (interval < 3600) {
        if (interval % 60 != 0 || 3600 % interval != 0) {
            sdsfree(cache_key);
            return reply_error_invalid_argument(ses, pkg);
        }
        result = get_market_kline_min(market, start, end, interval);
    } else if (interval < 86400) {
        if (interval % 3600 != 0 || 86400 % interval != 0) {
            sdsfree(cache_key);
            return reply_error_invalid_argument(ses, pkg);
        }
        result = get_market_kline_hour(market, start, end, interval);
    } else if (interval < 86400 * 7) {
        if (interval % 86400 != 0) {
            sdsfree(cache_key);
            return reply_error_invalid_argument(ses, pkg);
        }
        result = get_market_kline_day(market, start, end, interval);
    } else if (interval == 86400 * 7) {
        result = get_market_kline_week(market, start, end, interval);
    } else if (interval == 86400 * 30) {
        result = get_market_kline_month(market, start, end, interval);
    } else {
        sdsfree(cache_key);
        return reply_error_invalid_argument(ses, pkg);
    }

    if (result == NULL) {
        sdsfree(cache_key);
        return reply_error_internal_error(ses, pkg);
    }

    add_cache(cache_key, result);
    sdsfree(cache_key);

    int ret = reply_result(ses, pkg, result);
    json_decref(result);
    return ret;
}

static int on_cmd_market_deals(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 3)
        return reply_error_invalid_argument(ses, pkg);

    const char *market = json_string_value(json_array_get(params, 0));
    if (!market)
        return reply_error_invalid_argument(ses, pkg);
    if (!market_exist(market))
        return reply_error_invalid_argument(ses, pkg);

    int limit = json_integer_value(json_array_get(params, 1));
    if (limit <= 0 || limit > MARKET_DEALS_MAX)
        return reply_error_invalid_argument(ses, pkg);

    if (!json_is_integer(json_array_get(params, 2)))
        return reply_error_invalid_argument(ses, pkg);
    uint64_t last_id = json_integer_value(json_array_get(params, 2));

    json_t *result = get_market_deals(market, limit, last_id);
    if (result == NULL)
        return reply_error_internal_error(ses, pkg);

    int ret = reply_result(ses, pkg, result);
    json_decref(result);
    return ret;
}

static int on_cmd_market_last(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 1)
        return reply_error_invalid_argument(ses, pkg);

    const char *market = json_string_value(json_array_get(params, 0));
    if (!market)
        return reply_error_invalid_argument(ses, pkg);
    if (!market_exist(market))
        return reply_error_invalid_argument(ses, pkg);

    mpd_t *last = get_market_last_price(market);
    if (last == NULL)
        return reply_error_internal_error(ses, pkg);

    char *last_str = mpd_to_sci(last, 0);
    json_t *result = json_string(last_str);
    free(last_str);

    int ret = reply_result(ses, pkg, result);
    json_decref(result);
    return ret;
}

static int on_cmd_market_status_today(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 1)
        return reply_error_invalid_argument(ses, pkg);

    const char *market = json_string_value(json_array_get(params, 0));
    if (!market)
        return reply_error_invalid_argument(ses, pkg);
    if (!market_exist(market))
        return reply_error_invalid_argument(ses, pkg);

    sds cache_key = NULL;
    if (process_cache(ses, pkg, &cache_key))
        return 0;

    json_t *result = get_market_status_today(market);
    if (result == NULL) {
        sdsfree(cache_key);
        return reply_error_internal_error(ses, pkg);
    }

    add_cache(cache_key, result);
    sdsfree(cache_key);

    int ret = reply_result(ses, pkg, result);
    json_decref(result);
    return ret;
}

static void svr_on_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
    json_t *params = json_loadb(pkg->body, pkg->body_size, 0, NULL);
    if (params == NULL || !json_is_array(params)) {
        goto decode_error;
    }
    sds params_str = sdsnewlen(pkg->body, pkg->body_size);

    int ret;
    switch (pkg->command) {
    case CMD_MARKET_STATUS:
        log_debug("from: %s cmd market status, squence: %u params: %s", nw_sock_human_addr(&ses->peer_addr), pkg->sequence, params_str);
        ret = on_cmd_market_status(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_market_status %s fail: %d", params_str, ret);
        }
        break;
    case CMD_MARKET_KLINE:
        log_debug("from: %s cmd market kline, sequence: %u params: %s", nw_sock_human_addr(&ses->peer_addr), pkg->sequence, params_str);
        ret = on_cmd_market_kline(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_market_kline %s fail: %d", params_str, ret);
        }
        break;
    case CMD_MARKET_DEALS:
        log_debug("from: %s cmd market deals, sequence: %u params: %s", nw_sock_human_addr(&ses->peer_addr), pkg->sequence, params_str);
        ret = on_cmd_market_deals(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_market_deals %s fail: %d", params_str, ret);
        }
        break;
    case CMD_MARKET_LAST:
        log_debug("from: %s cmd market last, sequence: %u params: %s", nw_sock_human_addr(&ses->peer_addr), pkg->sequence, params_str);
        ret = on_cmd_market_last(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_market_last %s fail: %d", params_str, ret);
        }
        break;
    case CMD_MARKET_STATUS_TODAY:
        log_debug("from: %s cmd market today status, sequence: %u params: %s", nw_sock_human_addr(&ses->peer_addr), pkg->sequence, params_str);
        ret = on_cmd_market_status_today(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_market_status_today %s fail: %d", params_str, ret);
        }
        break;
    default:
        log_error("from: %s unknown command: %u", nw_sock_human_addr(&ses->peer_addr), pkg->command);
        break;
    }

    sdsfree(params_str);
    json_decref(params);
    return;

decode_error:
    if (params) {
        json_decref(params);
    }
    sds hex = hexdump(pkg->body, pkg->body_size);
    log_error("connection: %s, cmd: %u decode params fail, params data: \n%s", \
            nw_sock_human_addr(&ses->peer_addr), pkg->command, hex);
    sdsfree(hex);
    rpc_svr_close_clt(svr, ses);

    return;
}

static void svr_on_new_connection(nw_ses *ses)
{
    log_trace("new connection: %s", nw_sock_human_addr(&ses->peer_addr));
}

static void svr_on_connection_close(nw_ses *ses)
{
    log_trace("connection: %s close", nw_sock_human_addr(&ses->peer_addr));
}

static uint32_t cache_dict_hash_function(const void *key)
{
    return dict_generic_hash_function(key, sdslen((sds)key));
}

static int cache_dict_key_compare(const void *key1, const void *key2)
{
    return sdscmp((sds)key1, (sds)key2);
}

static void *cache_dict_key_dup(const void *key)
{
    return sdsdup((const sds)key);
}

static void cache_dict_key_free(void *key)
{
    sdsfree(key);
}

static void *cache_dict_val_dup(const void *val)
{
    struct cache_val *obj = malloc(sizeof(struct cache_val));
    memcpy(obj, val, sizeof(struct cache_val));
    return obj;
}

static void cache_dict_val_free(void *val)
{
    struct cache_val *obj = val;
    json_decref(obj->result);
    free(val);
}

static void on_cache_timer(nw_timer *timer, void *privdata)
{
    dict_clear(dict_cache);
}

int init_server(void)
{
    rpc_svr_type type;
    memset(&type, 0, sizeof(type));
    type.on_recv_pkg = svr_on_recv_pkg;
    type.on_new_connection = svr_on_new_connection;
    type.on_connection_close = svr_on_connection_close;

    svr = rpc_svr_create(&settings.svr, &type);
    if (svr == NULL)
        return -__LINE__;
    if (rpc_svr_start(svr) < 0)
        return -__LINE__;

    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function  = cache_dict_hash_function;
    dt.key_compare    = cache_dict_key_compare;
    dt.key_dup        = cache_dict_key_dup;
    dt.key_destructor = cache_dict_key_free;
    dt.val_dup        = cache_dict_val_dup;
    dt.val_destructor = cache_dict_val_free;

    dict_cache = dict_create(&dt, 64);
    if (dict_cache == NULL)
        return -__LINE__;

    nw_timer_set(&cache_timer, 60, true, on_cache_timer, NULL);
    nw_timer_start(&cache_timer);

    return 0;
}

