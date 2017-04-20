/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/18, create
 */

# include "mp_config.h"
# include "mp_server.h"
# include "mp_message.h"

static rpc_svr *svr;

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
    json_object_set_new(reply, "result", result);

    int ret = reply_json(ses, pkg, reply);
    json_decref(reply);

    return ret;
}

static int reply_success(nw_ses *ses, rpc_pkg *pkg)
{
    json_t *result = json_object();
    json_object_set_new(result, "status", json_string("success"));
    return reply_result(ses, pkg, result);
}

static int on_cmd_market_status(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    return 0;
}

static int on_cmd_market_kline(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    return 0;
}

static int on_cmd_market_deals(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    return 0;
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
        log_debug("from: %s cmd market status, params: %s", nw_sock_human_addr(&ses->peer_addr), params_str);
        ret = on_cmd_market_status(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_market_status %s fail: %d", params_str, ret);
        }
        break;
    case CMD_MARKET_KLINE:
        log_debug("from: %s cmd market kline, params: %s", nw_sock_human_addr(&ses->peer_addr), params_str);
        ret = on_cmd_market_kline(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_market_kline %s fail: %d", params_str, ret);
        }
        break;
    case CMD_MARKET_DEALS:
        log_debug("from: %s cmd market deals, params: %s", nw_sock_human_addr(&ses->peer_addr), params_str);
        ret = on_cmd_market_deals(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_market_deals %s fail: %d", params_str, ret);
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

    return 0;
}

