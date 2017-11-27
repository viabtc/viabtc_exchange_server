/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/21, create
 */

# include "aw_config.h"
# include "aw_server.h"
# include "aw_auth.h"
# include "aw_sign.h"
# include "aw_kline.h"
# include "aw_depth.h"
# include "aw_price.h"
# include "aw_state.h"
# include "aw_today.h"
# include "aw_deals.h"
# include "aw_order.h"
# include "aw_asset.h"

static ws_svr *svr;
static dict_t *method_map;
static dict_t *backend_cache;
static rpc_clt *listener;
static nw_state *state_context;
static nw_cache *privdata_cache;
static nw_timer cache_timer;

static rpc_clt *matchengine;
static rpc_clt *marketprice;
static rpc_clt *readhistory;

struct state_data {
    nw_ses      *ses;
    uint64_t    ses_id;
    uint64_t    request_id;
    sds         cache_key;
};

struct cache_val {
    double      time;
    json_t      *result;
};

typedef int (*on_request_method)(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params);

static int send_json(nw_ses *ses, const json_t *json)
{
    char *message_data = json_dumps(json, 0);
    if (message_data == NULL)
        return -__LINE__;
    log_trace("send to: %"PRIu64", size: %zu, message: %s", ses->id, strlen(message_data), message_data);
    int ret = ws_send_text(ses, message_data);
    free(message_data);
    return ret;
}

int send_error(nw_ses *ses, uint64_t id, int code, const char *message)
{
    json_t *error = json_object();
    json_object_set_new(error, "code", json_integer(code));
    json_object_set_new(error, "message", json_string(message));

    json_t *reply = json_object();
    json_object_set_new(reply, "error", error);
    json_object_set_new(reply, "result", json_null());
    json_object_set_new(reply, "id", json_integer(id));

    int ret = send_json(ses, reply);
    json_decref(reply);

    return ret;
}

int send_error_invalid_argument(nw_ses *ses, uint64_t id)
{
    return send_error(ses, id, 1, "invalid argument");
}

int send_error_internal_error(nw_ses *ses, uint64_t id)
{
    return send_error(ses, id, 2, "internal error");
}

int send_error_service_unavailable(nw_ses *ses, uint64_t id)
{
    return send_error(ses, id, 3, "service unavailable");
}

int send_error_method_notfound(nw_ses *ses, uint64_t id)
{
    return send_error(ses, id, 4, "method not found");
}

int send_error_service_timeout(nw_ses *ses, uint64_t id)
{
    return send_error(ses, id, 5, "service timeout");
}

int send_error_require_auth(nw_ses *ses, uint64_t id)
{
    return send_error(ses, id, 6, "require authentication");
}

int send_result(nw_ses *ses, uint64_t id, json_t *result)
{
    json_t *reply = json_object();
    json_object_set_new(reply, "error", json_null());
    json_object_set    (reply, "result", result);
    json_object_set_new(reply, "id", json_integer(id));

    int ret = send_json(ses, reply);
    json_decref(reply);

    return ret;
}

int send_success(nw_ses *ses, uint64_t id)
{
    json_t *result = json_object();
    json_object_set_new(result, "status", json_string("success"));

    int ret = send_result(ses, id, result);
    json_decref(result);

    return ret;
}

int send_notify(nw_ses *ses, const char *method, json_t *params)
{
    json_t *notify = json_object();
    json_object_set_new(notify, "method", json_string(method));
    json_object_set    (notify, "params", params);
    json_object_set_new(notify, "id", json_null());

    int ret = send_json(ses, notify);
    json_decref(notify);

    return ret;
}

static int on_method_server_ping(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    json_t *result = json_string("pong");
    int ret = send_result(ses, id, result);
    json_decref(result);
    return ret;
}

static int on_method_server_time(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    json_t *result = json_integer(time(NULL));
    int ret = send_result(ses, id, result);
    json_decref(result);
    return ret;
}

static int on_method_server_auth(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    return send_auth_request(ses, id, info, params);
}

static int on_method_server_sign(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    return send_sign_request(ses, id, info, params);
}

static int process_cache(nw_ses *ses, uint64_t id, sds key)
{
    dict_entry *entry = dict_find(backend_cache, key);
    if (entry == NULL)
        return 0;

    struct cache_val *cache = entry->val;
    double now = current_timestamp();
    if ((now - cache->time) > settings.cache_timeout) {
        dict_delete(backend_cache, key);
        return 0;
    }

    send_result(ses, id, cache->result);
    return 1;
}

static int on_method_kline_query(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!rpc_clt_connected(marketprice))
        return send_error_internal_error(ses, id);

    sds key = sdsempty();
    char *params_str = json_dumps(params, 0);
    key = sdscatprintf(key, "%u-%s", CMD_MARKET_KLINE, params_str);
    int ret = process_cache(ses, id, key);
    if (ret > 0) {
        sdsfree(key);
        free(params_str);
        return 0;
    }

    nw_state_entry *entry = nw_state_add(state_context, settings.backend_timeout, 0);
    struct state_data *state = entry->data;
    state->ses = ses;
    state->ses_id = ses->id;
    state->request_id = id;
    state->cache_key = key;

    rpc_pkg pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
    pkg.command   = CMD_MARKET_KLINE;
    pkg.sequence  = entry->id;
    pkg.req_id    = id;
    pkg.body      = params_str;
    pkg.body_size = strlen(pkg.body);

    rpc_clt_send(marketprice, &pkg);
    log_trace("send request to %s, cmd: %u, sequence: %u, params: %s",
            nw_sock_human_addr(rpc_clt_peer_addr(marketprice)), pkg.command, pkg.sequence, (char *)pkg.body);
    free(pkg.body);

    return 0;
}

static int on_method_kline_subscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (json_array_size(params) != 2)
        return send_error_invalid_argument(ses, id);

    const char *market = json_string_value(json_array_get(params, 0));
    int interval = json_integer_value(json_array_get(params, 1));
    if (market == NULL || strlen(market) >= MARKET_NAME_MAX_LEN || interval <= 0)
        return send_error_invalid_argument(ses, id);

    kline_unsubscribe(ses);
    if (kline_subscribe(ses, market, interval) < 0)
        return send_error_internal_error(ses, id);

    return send_success(ses, id);
}

static int on_method_kline_unsubscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    kline_unsubscribe(ses);
    return send_success(ses, id);
}

static int on_method_depth_query(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!rpc_clt_connected(matchengine))
        return send_error_internal_error(ses, id);

    sds key = sdsempty();
    char *params_str = json_dumps(params, 0);
    key = sdscatprintf(key, "%u-%s", CMD_ORDER_BOOK_DEPTH, params_str);
    int ret = process_cache(ses, id, key);
    if (ret > 0) {
        sdsfree(key);
        free(params_str);
        return 0;
    }

    nw_state_entry *entry = nw_state_add(state_context, settings.backend_timeout, 0);
    struct state_data *state = entry->data;
    state->ses = ses;
    state->ses_id = ses->id;
    state->request_id = id;
    state->cache_key = key;

    rpc_pkg pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
    pkg.command   = CMD_ORDER_BOOK_DEPTH;
    pkg.sequence  = entry->id;
    pkg.req_id    = id;
    pkg.body      = params_str;
    pkg.body_size = strlen(pkg.body);

    rpc_clt_send(matchengine, &pkg);
    log_trace("send request to %s, cmd: %u, sequence: %u, params: %s",
            nw_sock_human_addr(rpc_clt_peer_addr(matchengine)), pkg.command, pkg.sequence, (char *)pkg.body);
    free(pkg.body);

    return 0;
}

static int on_method_depth_subscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (json_array_size(params) != 3)
        return send_error_invalid_argument(ses, id);

    const char *market = json_string_value(json_array_get(params, 0));
    int limit = json_integer_value(json_array_get(params, 1));
    const char *interval = json_string_value(json_array_get(params, 2));
    if (market == NULL || strlen(market) >= MARKET_NAME_MAX_LEN || limit <= 0 || interval == NULL || strlen(interval) >= INTERVAL_MAX_LEN)
        return send_error_invalid_argument(ses, id);

    depth_unsubscribe(ses);
    int ret = depth_subscribe(ses, market, limit, interval);
    if (ret == -1) {
        return send_error_invalid_argument(ses, id);
    } else if (ret < 0) {
        return send_error_internal_error(ses, id);
    }

    send_success(ses, id);
    depth_send_clean(ses, market, limit, interval);
    return 0;
}

static int on_method_depth_unsubscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    depth_unsubscribe(ses);
    return send_success(ses, id);
}

static int on_method_price_query(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!rpc_clt_connected(marketprice))
        return send_error_internal_error(ses, id);

    sds key = sdsempty();
    char *params_str = json_dumps(params, 0);
    key = sdscatprintf(key, "%u-%s", CMD_MARKET_LAST, params_str);
    int ret = process_cache(ses, id, key);
    if (ret > 0) {
        sdsfree(key);
        free(params_str);
        return 0;
    }

    nw_state_entry *entry = nw_state_add(state_context, settings.backend_timeout, 0);
    struct state_data *state = entry->data;
    state->ses = ses;
    state->ses_id = ses->id;
    state->request_id = id;
    state->cache_key = key;

    rpc_pkg pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
    pkg.command   = CMD_MARKET_LAST;
    pkg.sequence  = entry->id;
    pkg.req_id    = id;
    pkg.body      = params_str;
    pkg.body_size = strlen(pkg.body);

    rpc_clt_send(marketprice, &pkg);
    log_trace("send request to %s, cmd: %u, sequence: %u, params: %s",
            nw_sock_human_addr(rpc_clt_peer_addr(marketprice)), pkg.command, pkg.sequence, (char *)pkg.body);
    free(pkg.body);

    return 0;
}

static int on_method_price_subscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    price_unsubscribe(ses);
    size_t params_size = json_array_size(params);
    for (size_t i = 0; i < params_size; ++i) {
        const char *market = json_string_value(json_array_get(params, i));
        if (market == NULL || strlen(market) >= MARKET_NAME_MAX_LEN)
            return send_error_invalid_argument(ses, id);
        if (price_subscribe(ses, market) < 0)
            return send_error_internal_error(ses, id);
    }

    send_success(ses, id);
    for (size_t i = 0; i < params_size; ++i) {
        price_send_last(ses, json_string_value(json_array_get(params, i)));
    }

    return 0;
}

static int on_method_price_unsubscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    price_unsubscribe(ses);
    return send_success(ses, id);
}

static int on_method_state_query(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!rpc_clt_connected(marketprice))
        return send_error_internal_error(ses, id);

    sds key = sdsempty();
    char *params_str = json_dumps(params, 0);
    key = sdscatprintf(key, "%u-%s", CMD_MARKET_STATUS, params_str);
    int ret = process_cache(ses, id, key);
    if (ret > 0) {
        sdsfree(key);
        free(params_str);
        return 0;
    }

    nw_state_entry *entry = nw_state_add(state_context, settings.backend_timeout, 0);
    struct state_data *state = entry->data;
    state->ses = ses;
    state->ses_id = ses->id;
    state->request_id = id;
    state->cache_key = key;

    rpc_pkg pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
    pkg.command   = CMD_MARKET_STATUS;
    pkg.sequence  = entry->id;
    pkg.req_id    = id;
    pkg.body      = params_str;
    pkg.body_size = strlen(pkg.body);

    rpc_clt_send(marketprice, &pkg);
    log_trace("send request to %s, cmd: %u, sequence: %u, params: %s",
            nw_sock_human_addr(rpc_clt_peer_addr(marketprice)), pkg.command, pkg.sequence, (char *)pkg.body);
    free(pkg.body);

    return 0;
}

static int on_method_state_subscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    state_unsubscribe(ses);
    size_t params_size = json_array_size(params);
    for (size_t i = 0; i < params_size; ++i) {
        const char *market = json_string_value(json_array_get(params, i));
        if (market == NULL || strlen(market) >= MARKET_NAME_MAX_LEN)
            return send_error_invalid_argument(ses, id);
        if (state_subscribe(ses, market) < 0)
            return send_error_internal_error(ses, id);
    }

    send_success(ses, id);
    for (size_t i = 0; i < params_size; ++i) {
        state_send_last(ses, json_string_value(json_array_get(params, i)));
    }

    return 0;
}

static int on_method_state_unsubscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    state_unsubscribe(ses);
    return send_success(ses, id);
}

static int on_method_today_query(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!rpc_clt_connected(marketprice))
        return send_error_internal_error(ses, id);

    sds key = sdsempty();
    char *params_str = json_dumps(params, 0);
    key = sdscatprintf(key, "%u-%s", CMD_MARKET_STATUS_TODAY, params_str);
    int ret = process_cache(ses, id, key);
    if (ret > 0) {
        sdsfree(key);
        free(params_str);
        return 0;
    }

    nw_state_entry *entry = nw_state_add(state_context, settings.backend_timeout, 0);
    struct state_data *state = entry->data;
    state->ses = ses;
    state->ses_id = ses->id;
    state->request_id = id;
    state->cache_key = key;

    rpc_pkg pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
    pkg.command   = CMD_MARKET_STATUS_TODAY;
    pkg.sequence  = entry->id;
    pkg.req_id    = id;
    pkg.body      = params_str;
    pkg.body_size = strlen(pkg.body);

    rpc_clt_send(marketprice, &pkg);
    log_trace("send request to %s, cmd: %u, sequence: %u, params: %s",
            nw_sock_human_addr(rpc_clt_peer_addr(marketprice)), pkg.command, pkg.sequence, (char *)pkg.body);
    free(pkg.body);

    return 0;
}

static int on_method_today_subscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    today_unsubscribe(ses);
    size_t params_size = json_array_size(params);
    for (size_t i = 0; i < params_size; ++i) {
        const char *market = json_string_value(json_array_get(params, i));
        if (market == NULL || strlen(market) >= MARKET_NAME_MAX_LEN)
            return send_error_invalid_argument(ses, id);
        if (today_subscribe(ses, market) < 0)
            return send_error_internal_error(ses, id);
    }

    send_success(ses, id);
    for (size_t i = 0; i < params_size; ++i) {
        today_send_last(ses, json_string_value(json_array_get(params, i)));
    }

    return 0;
}

static int on_method_today_unsubscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    today_unsubscribe(ses);
    return send_success(ses, id);
}

static int on_method_deals_query(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!rpc_clt_connected(marketprice))
        return send_error_internal_error(ses, id);

    sds key = sdsempty();
    char *params_str = json_dumps(params, 0);
    key = sdscatprintf(key, "%u-%s", CMD_MARKET_DEALS, params_str);
    int ret = process_cache(ses, id, key);
    if (ret > 0) {
        sdsfree(key);
        free(params_str);
        return 0;
    }

    nw_state_entry *entry = nw_state_add(state_context, settings.backend_timeout, 0);
    struct state_data *state = entry->data;
    state->ses = ses;
    state->ses_id = ses->id;
    state->request_id = id;
    state->cache_key = key;

    rpc_pkg pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
    pkg.command   = CMD_MARKET_DEALS;
    pkg.sequence  = entry->id;
    pkg.req_id    = id;
    pkg.body      = params_str;
    pkg.body_size = strlen(pkg.body);

    rpc_clt_send(marketprice, &pkg);
    log_trace("send request to %s, cmd: %u, sequence: %u, params: %s",
            nw_sock_human_addr(rpc_clt_peer_addr(marketprice)), pkg.command, pkg.sequence, (char *)pkg.body);
    free(pkg.body);

    return 0;
}

static int on_method_deals_subscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    deals_unsubscribe(ses);
    size_t params_size = json_array_size(params);
    for (size_t i = 0; i < params_size; ++i) {
        const char *market = json_string_value(json_array_get(params, i));
        if (market == NULL || strlen(market) >= MARKET_NAME_MAX_LEN)
            return send_error_invalid_argument(ses, id);
        if (deals_subscribe(ses, market) < 0)
            return send_error_internal_error(ses, id);
    }

    send_success(ses, id);
    for (size_t i = 0; i < params_size; ++i) {
        deals_send_full(ses, json_string_value(json_array_get(params, i)));
    }

    return 0;
}

static int on_method_deals_unsubscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    deals_unsubscribe(ses);
    return send_success(ses, id);
}

static int on_method_order_query(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!rpc_clt_connected(matchengine))
        return send_error_internal_error(ses, id);

    if (!info->auth)
        return send_error_require_auth(ses, id);
    if (json_array_size(params) != 3)
        return send_error_invalid_argument(ses, id);

    const char *market = json_string_value(json_array_get(params, 0));
    if (market == NULL)
        return send_error_invalid_argument(ses, id);
    if (!json_is_integer(json_array_get(params, 1)))
        return send_error_invalid_argument(ses, id);
    int offset = json_integer_value(json_array_get(params, 1));
    if (!json_is_integer(json_array_get(params, 2)))
        return send_error_invalid_argument(ses, id);
    int limit = json_integer_value(json_array_get(params, 2));

    json_t *trade_params = json_array();
    json_array_append_new(trade_params, json_integer(info->user_id));
    json_array_append_new(trade_params, json_string(market));
    json_array_append_new(trade_params, json_integer(offset));
    json_array_append_new(trade_params, json_integer(limit));

    nw_state_entry *entry = nw_state_add(state_context, settings.backend_timeout, 0);
    struct state_data *state = entry->data;
    state->ses = ses;
    state->ses_id = ses->id;
    state->request_id = id;

    rpc_pkg pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
    pkg.command   = CMD_ORDER_QUERY;
    pkg.sequence  = entry->id;
    pkg.req_id    = id;
    pkg.body      = json_dumps(trade_params, 0);
    pkg.body_size = strlen(pkg.body);

    rpc_clt_send(matchengine, &pkg);
    log_trace("send request to %s, cmd: %u, sequence: %u, params: %s",
            nw_sock_human_addr(rpc_clt_peer_addr(matchengine)), pkg.command, pkg.sequence, (char *)pkg.body);
    free(pkg.body);
    json_decref(trade_params);

    return 0;
}

static int on_method_order_history(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!rpc_clt_connected(readhistory))
        return send_error_internal_error(ses, id);

    if (!info->auth)
        return send_error_require_auth(ses, id);
    if (json_array_size(params) != 5)
        return send_error_invalid_argument(ses, id);

    json_t *read_params = json_array();
    json_array_append_new(read_params, json_integer(info->user_id));
    json_array_extend(read_params, params);

    nw_state_entry *entry = nw_state_add(state_context, settings.backend_timeout, 0);
    struct state_data *state = entry->data;
    state->ses = ses;
    state->ses_id = ses->id;
    state->request_id = id;

    rpc_pkg pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
    pkg.command   = CMD_ORDER_HISTORY;
    pkg.sequence  = entry->id;
    pkg.req_id    = id;
    pkg.body      = json_dumps(read_params, 0);
    pkg.body_size = strlen(pkg.body);

    rpc_clt_send(readhistory, &pkg);
    log_trace("send request to %s, cmd: %u, sequence: %u, params: %s",
            nw_sock_human_addr(rpc_clt_peer_addr(readhistory)), pkg.command, pkg.sequence, (char *)pkg.body);
    free(pkg.body);
    json_decref(read_params);

    return 0;
}

static int on_method_order_subscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!info->auth)
        return send_error_require_auth(ses, id);

    order_unsubscribe(info->user_id, ses);
    size_t params_size = json_array_size(params);
    for (size_t i = 0; i < params_size; ++i) {
        const char *market = json_string_value(json_array_get(params, i));
        if (market == NULL || strlen(market) >= MARKET_NAME_MAX_LEN)
            return send_error_invalid_argument(ses, id);
        if (order_subscribe(info->user_id, ses, market) < 0)
            return send_error_internal_error(ses, id);
    }

    return send_success(ses, id);
}

static int on_method_order_unsubscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!info->auth)
        return send_error_require_auth(ses, id);

    order_unsubscribe(info->user_id, ses);
    return send_success(ses, id);
}

static int on_method_asset_query(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!info->auth)
        return send_error_require_auth(ses, id);

    if (!rpc_clt_connected(matchengine))
        return send_error_internal_error(ses, id);

    json_t *trade_params = json_array();
    json_array_append_new(trade_params, json_integer(info->user_id));
    json_array_extend(trade_params, params);

    nw_state_entry *entry = nw_state_add(state_context, settings.backend_timeout, 0);
    struct state_data *state = entry->data;
    state->ses = ses;
    state->ses_id = ses->id;
    state->request_id = id;

    rpc_pkg pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
    pkg.command   = CMD_BALANCE_QUERY;
    pkg.sequence  = entry->id;
    pkg.req_id    = id;
    pkg.body      = json_dumps(trade_params, 0);
    pkg.body_size = strlen(pkg.body);

    rpc_clt_send(matchengine, &pkg);
    log_trace("send request to %s, cmd: %u, sequence: %u, params: %s",
            nw_sock_human_addr(rpc_clt_peer_addr(matchengine)), pkg.command, pkg.sequence, (char *)pkg.body);
    free(pkg.body);
    json_decref(trade_params);

    return 0;
}

static int on_method_asset_history(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!rpc_clt_connected(readhistory))
        return send_error_internal_error(ses, id);

    if (!info->auth)
        return send_error_require_auth(ses, id);
    if (json_array_size(params) != 6)
        return send_error_invalid_argument(ses, id);

    json_t *read_params = json_array();
    json_array_append_new(read_params, json_integer(info->user_id));
    json_array_extend(read_params, params);

    nw_state_entry *entry = nw_state_add(state_context, settings.backend_timeout, 0);
    struct state_data *state = entry->data;
    state->ses = ses;
    state->ses_id = ses->id;
    state->request_id = id;

    rpc_pkg pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
    pkg.command   = CMD_BALANCE_HISTORY;
    pkg.sequence  = entry->id;
    pkg.req_id    = id;
    pkg.body      = json_dumps(read_params, 0);
    pkg.body_size = strlen(pkg.body);

    rpc_clt_send(readhistory, &pkg);
    log_trace("send request to %s, cmd: %u, sequence: %u, params: %s",
            nw_sock_human_addr(rpc_clt_peer_addr(readhistory)), pkg.command, pkg.sequence, (char *)pkg.body);
    free(pkg.body);
    json_decref(read_params);

    return 0;
}

static int on_method_asset_subscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!info->auth)
        return send_error_require_auth(ses, id);

    asset_unsubscribe(info->user_id, ses);
    size_t params_size = json_array_size(params);
    for (size_t i = 0; i < params_size; ++i) {
        const char *asset = json_string_value(json_array_get(params, i));
        if (asset == NULL || strlen(asset) >= ASSET_NAME_MAX_LEN)
            return send_error_invalid_argument(ses, id);
        if (asset_subscribe(info->user_id, ses, asset) < 0)
            return send_error_internal_error(ses, id);
    }

    return send_success(ses, id);
}

static int on_method_asset_unsubscribe(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (!info->auth)
        return send_error_require_auth(ses, id);

    asset_unsubscribe(info->user_id, ses);
    return send_success(ses, id);
}

static int on_message(nw_ses *ses, const char *remote, const char *url, void *message, size_t size)
{
    struct clt_info *info = ws_ses_privdata(ses);
    log_trace("new websocket message from: %"PRIu64":%s, url: %s, size: %zu", ses->id, remote, url, size);
    json_t *msg = json_loadb(message, size, 0, NULL);
    if (msg == NULL) {
        goto decode_error;
    }

    json_t *id = json_object_get(msg, "id");
    if (!id || !json_is_integer(id)) {
        goto decode_error;
    }
    json_t *method = json_object_get(msg, "method");
    if (!method || !json_is_string(method)) {
        goto decode_error;
    }
    json_t *params = json_object_get(msg, "params");
    if (!params || !json_is_array(params)) {
        goto decode_error;
    }

    sds _msg = sdsnewlen(message, size);
    log_trace("remote: %"PRIu64":%s message: %s", ses->id, remote, _msg);

    uint64_t _id = json_integer_value(id);
    const char *_method = json_string_value(method);
    dict_entry *entry = dict_find(method_map, _method);
    if (entry) {
        on_request_method handler = entry->val;
        int ret = handler(ses, _id, info, params);
        if (ret < 0) {
            log_error("remote: %"PRIu64":%s, request fail: %d, request: %s", ses->id, remote, ret, _msg);
        }
    } else {
        log_error("remote: %"PRIu64":%s, unknown method, request: %s", ses->id, remote, _msg);
    }

    sdsfree(_msg);
    json_decref(msg);

    return 0;

decode_error:
    if (msg)
        json_decref(msg);
    sds hex = hexdump(message, size);
    log_error("remote: %"PRIu64":%s, decode request fail, request body: \n%s", ses->id, remote, hex);
    sdsfree(hex);
    return -__LINE__;
}

static void on_upgrade(nw_ses *ses, const char *remote)
{
    log_trace("remote: %"PRIu64":%s upgrade to websocket", ses->id, remote);
    struct clt_info *info = ws_ses_privdata(ses);
    memset(info, 0, sizeof(struct clt_info));
}

static void on_close(nw_ses *ses, const char *remote)
{
    log_trace("remote: %"PRIu64":%s websocket connection close", ses->id, remote);

    kline_unsubscribe(ses);
    depth_unsubscribe(ses);
    price_unsubscribe(ses);
    today_unsubscribe(ses);
    deals_unsubscribe(ses);

    struct clt_info *info = ws_ses_privdata(ses);
    if (info->auth) {
        order_unsubscribe(info->user_id, ses);
        asset_unsubscribe(info->user_id, ses);
    }
}

static void *on_privdata_alloc(void *svr)
{
    return nw_cache_alloc(privdata_cache);
}

static void on_privdata_free(void *svr, void *privdata)
{
    struct clt_info *info = privdata;
    if (info->source)
        free(info->source);
    nw_cache_free(privdata_cache, privdata);
}

static uint32_t dict_method_hash_func(const void *key)
{
    return dict_generic_hash_function(key, strlen(key));
}

static int dict_method_key_compare(const void *key1, const void *key2)
{
    return strcmp(key1, key2);
}

static void *dict_method_key_dup(const void *key)
{
    return strdup(key);
}

static void dict_method_key_free(void *key)
{
    free(key);
}

static int add_handler(char *method, on_request_method func)
{
    if (dict_add(method_map, method, func) == NULL)
        return __LINE__;
    return 0;
}

static void on_timeout(nw_state_entry *entry)
{
    log_error("state id: %u timeout", entry->id);
    struct state_data *state = entry->data;
    if (state->ses->id == state->ses_id) {
        send_error_service_timeout(state->ses, state->request_id);
    }
}

static void on_release(nw_state_entry *entry)
{
    struct state_data *state = entry->data;
    if (state->cache_key)
        sdsfree(state->cache_key);
}

static int init_svr(void)
{
    ws_svr_type type;
    memset(&type, 0, sizeof(type));
    type.on_upgrade = on_upgrade;
    type.on_close = on_close;
    type.on_message = on_message;
    type.on_privdata_alloc = on_privdata_alloc;
    type.on_privdata_free = on_privdata_free;

    svr = ws_svr_create(&settings.svr, &type);
    if (svr == NULL)
        return -__LINE__;

    privdata_cache = nw_cache_create(sizeof(struct clt_info));
    if (privdata_cache == NULL)
        return -__LINE__;

    nw_state_type st;
    memset(&st, 0, sizeof(st));
    st.on_timeout = on_timeout;
    st.on_release = on_release;

    state_context = nw_state_create(&st, sizeof(struct state_data));
    if (state_context == NULL)
        return -__LINE__;

    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function = dict_method_hash_func;
    dt.key_compare = dict_method_key_compare;
    dt.key_dup = dict_method_key_dup;
    dt.key_destructor = dict_method_key_free;

    method_map = dict_create(&dt, 64);
    if (method_map == NULL)
        return -__LINE__;

    ERR_RET_LN(add_handler("server.ping",       on_method_server_ping));
    ERR_RET_LN(add_handler("server.time",       on_method_server_time));
    ERR_RET_LN(add_handler("server.auth",       on_method_server_auth));
    ERR_RET_LN(add_handler("server.sign",       on_method_server_sign));

    ERR_RET_LN(add_handler("kline.query",       on_method_kline_query));
    ERR_RET_LN(add_handler("kline.subscribe",   on_method_kline_subscribe));
    ERR_RET_LN(add_handler("kline.unsubscribe", on_method_kline_unsubscribe));

    ERR_RET_LN(add_handler("depth.query",       on_method_depth_query));
    ERR_RET_LN(add_handler("depth.subscribe",   on_method_depth_subscribe));
    ERR_RET_LN(add_handler("depth.unsubscribe", on_method_depth_unsubscribe));

    ERR_RET_LN(add_handler("price.query",       on_method_price_query));
    ERR_RET_LN(add_handler("price.subscribe",   on_method_price_subscribe));
    ERR_RET_LN(add_handler("price.unsubscribe", on_method_price_unsubscribe));

    ERR_RET_LN(add_handler("state.query",       on_method_state_query));
    ERR_RET_LN(add_handler("state.subscribe",   on_method_state_subscribe));
    ERR_RET_LN(add_handler("state.unsubscribe", on_method_state_unsubscribe));

    ERR_RET_LN(add_handler("today.query",       on_method_today_query));
    ERR_RET_LN(add_handler("today.subscribe",   on_method_today_subscribe));
    ERR_RET_LN(add_handler("today.unsubscribe", on_method_today_unsubscribe));

    ERR_RET_LN(add_handler("deals.query",       on_method_deals_query));
    ERR_RET_LN(add_handler("deals.subscribe",   on_method_deals_subscribe));
    ERR_RET_LN(add_handler("deals.unsubscribe", on_method_deals_unsubscribe));

    ERR_RET_LN(add_handler("order.query",       on_method_order_query));
    ERR_RET_LN(add_handler("order.history",     on_method_order_history));
    ERR_RET_LN(add_handler("order.subscribe",   on_method_order_subscribe));
    ERR_RET_LN(add_handler("order.unsubscribe", on_method_order_unsubscribe));

    ERR_RET_LN(add_handler("asset.query",       on_method_asset_query));
    ERR_RET_LN(add_handler("asset.history",     on_method_asset_history));
    ERR_RET_LN(add_handler("asset.subscribe",   on_method_asset_subscribe));
    ERR_RET_LN(add_handler("asset.unsubscribe", on_method_asset_unsubscribe));

    return 0;
}

static void on_backend_connect(nw_ses *ses, bool result)
{
    rpc_clt *clt = ses->privdata;
    if (result) {
        log_info("connect %s:%s success", clt->name, nw_sock_human_addr(&ses->peer_addr));
    } else {
        log_info("connect %s:%s fail", clt->name, nw_sock_human_addr(&ses->peer_addr));
    }
}

static void on_backend_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
    log_trace("recv pkg from: %s, cmd: %u, sequence: %u",
            nw_sock_human_addr(&ses->peer_addr), pkg->command, pkg->sequence);
    nw_state_entry *entry = nw_state_get(state_context, pkg->sequence);
    if (entry == NULL)
        return;

    struct state_data *state = entry->data;
    if (state->ses->id == state->ses_id) {
        sds message = sdsnewlen(pkg->body, pkg->body_size);
        log_trace("send response to: %"PRIu64", size: %zu, message: %s", state->ses->id, sdslen(message), message);
        ws_send_text(state->ses, message);
        sdsfree(message);
    }
    if (state->cache_key) {
        json_t *reply = json_loadb(pkg->body, pkg->body_size, 0, NULL);
        if (reply && json_is_object(reply)) {
            json_t *result = json_object_get(reply, "result");
            if (result && !json_is_null(result)) {
                struct cache_val val;
                val.time = current_timestamp();
                val.result = result;
                json_incref(result);
                dict_replace(backend_cache, state->cache_key, &val);
            }
        }
        if (reply) {
            json_decref(reply);
        }
    }
    nw_state_del(state_context, pkg->sequence);
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
    dict_clear(backend_cache);
}

static int init_backend(void)
{
    rpc_clt_type ct;
    memset(&ct, 0, sizeof(ct));
    ct.on_connect = on_backend_connect;
    ct.on_recv_pkg = on_backend_recv_pkg;

    matchengine = rpc_clt_create(&settings.matchengine, &ct);
    if (matchengine == NULL)
        return -__LINE__;
    if (rpc_clt_start(matchengine) < 0)
        return -__LINE__;

    marketprice = rpc_clt_create(&settings.marketprice, &ct);
    if (marketprice == NULL)
        return -__LINE__;
    if (rpc_clt_start(marketprice) < 0)
        return -__LINE__;

    readhistory = rpc_clt_create(&settings.readhistory, &ct);
    if (readhistory == NULL)
        return -__LINE__;
    if (rpc_clt_start(readhistory) < 0)
        return -__LINE__;

    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function  = cache_dict_hash_function;
    dt.key_compare    = cache_dict_key_compare;
    dt.key_dup        = cache_dict_key_dup;
    dt.key_destructor = cache_dict_key_free;
    dt.val_dup        = cache_dict_val_dup;
    dt.val_destructor = cache_dict_val_free;

    backend_cache = dict_create(&dt, 64);
    if (backend_cache == NULL)
        return -__LINE__;

    nw_timer_set(&cache_timer, 60, true, on_cache_timer, NULL);
    nw_timer_start(&cache_timer);

    return 0;
}

static void on_listener_connect(nw_ses *ses, bool result)
{
    if (result) {
        log_info("connect listener success");
    } else {
        log_info("connect listener fail");
    }
}

static void on_listener_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
    return;
}

static void on_listener_recv_fd(nw_ses *ses, int fd)
{
    if (nw_svr_add_clt_fd(svr->raw_svr, fd) < 0) {
        log_error("nw_svr_add_clt_fd: %d fail: %s", fd, strerror(errno));
        close(fd);
    }
}

static int init_listener_clt(void)
{
    rpc_clt_cfg cfg;
    nw_addr_t addr;
    memset(&cfg, 0, sizeof(cfg));
    cfg.name = strdup("listener");
    cfg.addr_count = 1;
    cfg.addr_arr = &addr;
    if (nw_sock_cfg_parse(AW_LISTENER_BIND, &addr, &cfg.sock_type) < 0)
        return -__LINE__;
    cfg.max_pkg_size = 1024;

    rpc_clt_type type;
    memset(&type, 0, sizeof(type));
    type.on_connect  = on_listener_connect;
    type.on_recv_pkg = on_listener_recv_pkg;
    type.on_recv_fd  = on_listener_recv_fd;

    listener = rpc_clt_create(&cfg, &type);
    if (listener == NULL)
        return -__LINE__;
    if (rpc_clt_start(listener) < 0)
        return -__LINE__;

    return 0;
}

int init_server(void)
{
    ERR_RET(init_svr());
    ERR_RET(init_backend());
    ERR_RET(init_listener_clt());

    return 0;
}

