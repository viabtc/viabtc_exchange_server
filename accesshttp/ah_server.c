/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/21, create
 */

# include "ah_config.h"
# include "ah_server.h"

static http_svr *svr;
static nw_state *state;
static dict_t *methods;
static rpc_clt *listener;

static rpc_clt *matchengine;
static rpc_clt *marketprice;
static rpc_clt *readhistory;

struct state_info {
    nw_ses  *ses;
    uint64_t ses_id;
    int64_t  request_id;
};

struct request_info {
    rpc_clt *clt;
    uint32_t cmd;
};

static void reply_error(nw_ses *ses, int64_t id, int code, const char *message, uint32_t status)
{
    json_t *error = json_object();
    json_object_set_new(error, "code", json_integer(code));
    json_object_set_new(error, "message", json_string(message));
    json_t *reply = json_object();
    json_object_set_new(reply, "error", error);
    json_object_set_new(reply, "result", json_null());
    json_object_set_new(reply, "id", json_integer(id));

    char *reply_str = json_dumps(reply, 0);
    send_http_response_simple(ses, status, reply_str, strlen(reply_str));
    free(reply_str);
    json_decref(reply);
}

static void reply_bad_request(nw_ses *ses)
{
    send_http_response_simple(ses, 400, NULL, 0);
}

static void reply_internal_error(nw_ses *ses)
{
    send_http_response_simple(ses, 500, NULL, 0);
}

static void reply_not_found(nw_ses *ses, int64_t id)
{
    reply_error(ses, id, 4, "method not found", 404);
}

static void reply_time_out(nw_ses *ses, int64_t id)
{
    reply_error(ses, id, 5, "service timeout", 504);
}

static int on_http_request(nw_ses *ses, http_request_t *request)
{
    log_trace("new http request, url: %s, method: %u", request->url, request->method);
    if (request->method != HTTP_POST || !request->body) {
        reply_bad_request(ses);
        return -__LINE__;
    }

    json_t *body = json_loadb(request->body, sdslen(request->body), 0, NULL);
    if (body == NULL) {
        goto decode_error;
    }
    json_t *id = json_object_get(body, "id");
    if (!id || !json_is_integer(id)) {
        goto decode_error;
    }
    json_t *method = json_object_get(body, "method");
    if (!method || !json_is_string(method)) {
        goto decode_error;
    }
    json_t *params = json_object_get(body, "params");
    if (!params || !json_is_array(params)) {
        goto decode_error;
    }
    log_trace("from: %s body: %s", nw_sock_human_addr(&ses->peer_addr), request->body);

    dict_entry *entry = dict_find(methods, json_string_value(method));
    if (entry == NULL) {
        reply_not_found(ses, json_integer_value(id));
    } else {
        struct request_info *req = entry->val;
        if (!rpc_clt_connected(req->clt)) {
            reply_internal_error(ses);
            json_decref(body);
            return 0;
        }

        nw_state_entry *entry = nw_state_add(state, settings.timeout, 0);
        struct state_info *info = entry->data;
        info->ses = ses;
        info->ses_id = ses->id;
        info->request_id = json_integer_value(id);

        rpc_pkg pkg;
        memset(&pkg, 0, sizeof(pkg));
        pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
        pkg.command   = req->cmd;
        pkg.sequence  = entry->id;
        pkg.req_id    = json_integer_value(id);
        pkg.body      = json_dumps(params, 0);
        pkg.body_size = strlen(pkg.body);

        rpc_clt_send(req->clt, &pkg);
        log_debug("send request to %s, cmd: %u, sequence: %u",
                nw_sock_human_addr(rpc_clt_peer_addr(req->clt)), pkg.command, pkg.sequence);
        free(pkg.body);
    }

    json_decref(body);
    return 0;

decode_error:
    if (body)
        json_decref(body);
    sds hex = hexdump(request->body, sdslen(request->body));
    log_fatal("peer: %s, decode request fail, request body: \n%s", nw_sock_human_addr(&ses->peer_addr), hex);
    sdsfree(hex);
    reply_bad_request(ses);
    return -__LINE__;
}

static uint32_t dict_hash_func(const void *key)
{
    return dict_generic_hash_function(key, strlen(key));
}

static int dict_key_compare(const void *key1, const void *key2)
{
    return strcmp(key1, key2);
}

static void *dict_key_dup(const void *key)
{
    return strdup(key);
}

static void dict_key_free(void *key)
{
    free(key);
}

static void *dict_val_dup(const void *val)
{
    struct request_info *obj = malloc(sizeof(struct request_info));
    memcpy(obj, val, sizeof(struct request_info));
    return obj;
}

static void dict_val_free(void *val)
{
    free(val);
}

static void on_state_timeout(nw_state_entry *entry)
{
    log_error("state id: %u timeout", entry->id);
    struct state_info *info = entry->data;
    if (info->ses->id == info->ses_id) {
        reply_time_out(info->ses, info->request_id);
    }
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
    log_debug("recv pkg from: %s, cmd: %u, sequence: %u",
            nw_sock_human_addr(&ses->peer_addr), pkg->command, pkg->sequence);
    nw_state_entry *entry = nw_state_get(state, pkg->sequence);
    if (entry) {
        struct state_info *info = entry->data;
        if (info->ses->id == info->ses_id) {
            log_trace("send response to: %s", nw_sock_human_addr(&info->ses->peer_addr));
            send_http_response_simple(info->ses, 200, pkg->body, pkg->body_size);
        }
        nw_state_del(state, pkg->sequence);
    }
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
    if (nw_sock_cfg_parse(AH_LISTENER_BIND, &addr, &cfg.sock_type) < 0)
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

static int add_handler(char *method, rpc_clt *clt, uint32_t cmd)
{
    struct request_info info = { .clt = clt, .cmd = cmd };
    if (dict_add(methods, method, &info) == NULL)
        return __LINE__;
    return 0;
}

static int init_methods_handler(void)
{
    ERR_RET_LN(add_handler("asset.list", matchengine, CMD_ASSET_LIST));
    ERR_RET_LN(add_handler("asset.summary", matchengine, CMD_ASSET_SUMMARY));

    ERR_RET_LN(add_handler("balance.query", matchengine, CMD_BALANCE_QUERY));
    ERR_RET_LN(add_handler("balance.update", matchengine, CMD_BALANCE_UPDATE));
    ERR_RET_LN(add_handler("balance.history", readhistory, CMD_BALANCE_HISTORY));

    ERR_RET_LN(add_handler("order.put_limit", matchengine, CMD_ORDER_PUT_LIMIT));
    ERR_RET_LN(add_handler("order.put_market", matchengine, CMD_ORDER_PUT_MARKET));
    ERR_RET_LN(add_handler("order.cancel", matchengine, CMD_ORDER_CANCEL));
    ERR_RET_LN(add_handler("order.book", matchengine, CMD_ORDER_BOOK));
    ERR_RET_LN(add_handler("order.depth", matchengine, CMD_ORDER_BOOK_DEPTH));
    ERR_RET_LN(add_handler("order.pending", matchengine, CMD_ORDER_QUERY));
    ERR_RET_LN(add_handler("order.pending_detail", matchengine, CMD_ORDER_DETAIL));
    ERR_RET_LN(add_handler("order.deals", readhistory, CMD_ORDER_DEALS));
    ERR_RET_LN(add_handler("order.finished", readhistory, CMD_ORDER_HISTORY));
    ERR_RET_LN(add_handler("order.finished_detail", readhistory, CMD_ORDER_DETAIL_FINISHED));

    ERR_RET_LN(add_handler("market.last", marketprice, CMD_MARKET_LAST));
    ERR_RET_LN(add_handler("market.deals", marketprice, CMD_MARKET_DEALS));
    ERR_RET_LN(add_handler("market.kline", marketprice, CMD_MARKET_KLINE));
    ERR_RET_LN(add_handler("market.status", marketprice, CMD_MARKET_STATUS));
    ERR_RET_LN(add_handler("market.status_today", marketprice, CMD_MARKET_STATUS_TODAY));
    ERR_RET_LN(add_handler("market.user_deals", readhistory, CMD_MARKET_USER_DEALS));
    ERR_RET_LN(add_handler("market.list", matchengine, CMD_MARKET_LIST));
    ERR_RET_LN(add_handler("market.summary", matchengine, CMD_MARKET_SUMMARY));

    return 0;
}

int init_server(void)
{
    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function = dict_hash_func;
    dt.key_compare = dict_key_compare;
    dt.key_dup = dict_key_dup;
    dt.val_dup = dict_val_dup;
    dt.key_destructor = dict_key_free;
    dt.val_destructor = dict_val_free;
    methods = dict_create(&dt, 64);
    if (methods == NULL)
        return -__LINE__;

    nw_state_type st;
    memset(&st, 0, sizeof(st));
    st.on_timeout = on_state_timeout;
    state = nw_state_create(&st, sizeof(struct state_info));
    if (state == NULL)
        return -__LINE__;

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

    svr = http_svr_create(&settings.svr, on_http_request);
    if (svr == NULL)
        return -__LINE__;

    ERR_RET(init_methods_handler());
    ERR_RET(init_listener_clt());

    return 0;
}

