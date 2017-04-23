/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/21, create
 */

# include "ah_config.h"
# include "ah_server.h"

static dict_t *methods;
static nw_state *state;
static rpc_clt *matchengine;
static rpc_clt *marketprice;
static http_svr *svr;

struct state_info {
    nw_ses  *ses;
    uint64_t ses_id;
    int64_t  request_id;
};

typedef void (*on_method)(nw_ses *ses, json_t *params, int64_t id);

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
    sds response = sdsnew(reply_str);
    send_http_response_simple(ses, status, response);
    free(reply_str);
    sdsfree(response);
    json_decref(reply);
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
    if (request->method != HTTP_POST || !request->body) {
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

    dict_entry *entry = dict_find(methods, json_string_value(method));
    if (!entry) {
        reply_not_found(ses, json_integer_value(id));
    } else {
        on_method callback = entry->val;
        callback(ses, params, json_integer_value(id));
    }

    json_decref(body);
    return 0;

decode_error:
    if (body)
        json_decref(body);
    sds hex = hexdump(request->body, sdslen(request->body));
    log_error("peer: %s, decode request fail, request body: \n%s", nw_sock_human_addr(&ses->peer_addr), hex);
    sdsfree(hex);
    return -1;
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

static void on_state_timeout(nw_state_entry *entry)
{
    struct state_info *info = entry->data;
    if (info->ses->id == info->ses_id) {
        reply_time_out(info->ses, info->request_id);
    }
}

static void on_connect(nw_ses *ses, bool result)
{
    rpc_clt *clt = ses->privdata;
    if (result) {
        log_info("connecte %s:%s success", clt->name, nw_sock_human_addr(&ses->peer_addr));
    } else {
        log_info("connecte %s:%s fail", clt->name, nw_sock_human_addr(&ses->peer_addr));
    }
}

static void on_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
    nw_state_entry *entry = nw_state_get(state, pkg->sequence);
    if (entry) {
        struct state_info *info = entry->data;
        if (info->ses->id == info->ses_id) {
            sds response = sdsnewlen(pkg->body, pkg->body_size);
            send_http_response_simple(info->ses, 200, response);
            sdsfree(response);
        }
    }
}

int init_server(void)
{
    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function = dict_hash_func;
    dt.key_dup = dict_key_dup;
    dt.key_destructor = dict_key_free;
    dt.key_compare = dict_key_compare;
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
    ct.on_connect = on_connect;
    ct.on_recv_pkg = on_recv_pkg;
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

    svr = http_svr_create(&settings.svr, on_http_request);
    if (svr == NULL)
        return -__LINE__;
    if (http_svr_start(svr) < 0)
        return -__LINE__;

    return 0;
}

