/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/21, create
 */

# include "aw_config.h"
# include "aw_server.h"

static ws_svr *svr;
static nw_cache *privdata_cache;

static int reply_json(nw_ses *ses, const json_t *json)
{
    char *message_data = json_dumps(json, 0);
    if (message_data == NULL)
        return -__LINE__;
    int ret = ws_send_text(ses, message_data);
    free(message_data);
    return ret;
}

static int reply_error(nw_ses *ses, uint64_t id, int code, const char *message)
{
    json_t *error = json_object();
    json_object_set_new(error, "code", json_integer(code));
    json_object_set_new(error, "message", json_string(message));

    json_t *reply = json_object();
    json_object_set_new(reply, "error", error);
    json_object_set_new(reply, "result", json_null());
    json_object_set_new(reply, "id", json_integer(id));

    int ret = reply_json(ses, reply);
    json_decref(reply);

    return ret;
}

static int reply_error_invalid_argument(nw_ses *ses, uint64_t id)
{
    return reply_error(ses, id, 1, "invalid argument");
}

static int reply_error_internal_error(nw_ses *ses, uint64_t id)
{
    return reply_error(ses, id, 2, "internal error");
}

static int reply_error_service_unavailable(nw_ses *ses, uint64_t id)
{
    return reply_error(ses, id, 3, "service unavailable");
}

static int reply_result(nw_ses *ses, uint64_t id, json_t *result)
{
    json_t *reply = json_object();
    json_object_set_new(reply, "error", json_null());
    json_object_set_new(reply, "result", result);
    json_object_set_new(reply, "id", json_integer(id));

    int ret = reply_json(ses, reply);
    json_decref(reply);

    return ret;
}

static int reply_success(nw_ses *ses, uint64_t id)
{
    json_t *result = json_object();
    json_object_set_new(result, "status", json_string("success"));
    return reply_result(ses, id, result);
}

static int on_server_time(nw_ses *ses, struct clt_info *info, uint64_t id, json_t *params)
{
    return reply_result(ses, id, json_integer(time(NULL)));
}

static int on_message(nw_ses *ses, const char *remote, const char *url, void *message, size_t size)
{
    struct clt_info *info = ws_ses_privdata(ses);
    log_trace("new websocket message from: %"PRIu64":%s, url: %s", ses->id, remote, url);
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

    int ret;
    uint64_t _id = json_integer_value(id);
    const char *_method = json_string_value(method);
    if (strcmp(_method, "server.time") == 0) {
        ret = on_server_time(ses, info, _id, params);
    } else {
        log_error("remote: %"PRIu64":%s, unknown method, request: %s", ses->id, remote, _msg);
    }

    if (ret < 0) {
        log_error("remote: %"PRIu64":%s, request fail: %d, request: %s", ses->id, remote, ret, _msg);
        return -__LINE__;
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
}

static void *on_privdata_alloc(void *svr)
{
    return nw_cache_alloc(privdata_cache);
}

static void on_privdata_free(void *svr, void *privdata)
{
    struct clt_info *info = privdata;
    if (info->taker_fee)
        mpd_del(info->taker_fee);
    if (info->maker_fee)
        mpd_del(info->maker_fee);
    nw_cache_free(privdata_cache, privdata);
}

int init_server(int worker_id)
{
    ws_svr_type type;
    memset(&type, 0, sizeof(type));
    type.on_upgrade = on_upgrade;
    type.on_close = on_close;
    type.on_message = on_message;
    type.on_privdata_alloc = on_privdata_alloc;
    type.on_privdata_free = on_privdata_free;

    for (uint32_t i = 0; i < settings.svr.bind_count; ++i) {
        nw_addr_t *addr = &(settings.svr.bind_arr[i].addr);
        if (addr->family == AF_INET) {
            addr->in.sin_port = htons(ntohs(addr->in.sin_port) + worker_id);
        } else if (addr->family == AF_INET6) {
            addr->in6.sin6_port = htons(ntohs(addr->in6.sin6_port) + worker_id);
        }
    }

    svr = ws_svr_create(&settings.svr, &type);
    if (svr == NULL)
        return -__LINE__;
    if (ws_svr_start(svr) < 0)
        return -__LINE__;

    privdata_cache = nw_cache_create(sizeof(struct clt_info));
    if (privdata_cache == NULL)
        return -__LINE__;

    return 0;
}

