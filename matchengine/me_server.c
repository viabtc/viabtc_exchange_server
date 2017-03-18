/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/16, create
 */

# include "me_config.h"
# include "me_server.h"
# include "me_update.h"
# include "me_balance.h"

static rpc_svr *svr;

static int reply_json(nw_ses *ses, rpc_pkg *pkg, const json_t *json)
{
    char *message_data = json_dumps(json, 0);
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

static int on_cmd_balance_query(nw_ses *ses, rpc_pkg *pkg, json_t *request)
{
    size_t request_size = json_array_size(request);
    if (request_size == 0 || request_size > 2)
        return reply_error_invalid_argument(ses, pkg);

    if (!json_is_integer(json_array_get(request, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(request, 0));

    if (request_size == 2) {
        if (!json_is_string(json_array_get(request, 1)))
            return reply_error_invalid_argument(ses, pkg);
        const char *asset = json_string_value(json_array_get(request, 1));
        if (asset_exist(asset) < 0)
            return reply_error_invalid_argument(ses, pkg);

        json_t *unit = json_object();
        mpd_t *available = balance_get(user_id, BALANCE_TYPE_AVAILABLE, asset);
        json_object_set_new(unit, "available", json_string(available == NULL ? "0" : mpd_to_sci(available, 0)));
        mpd_t *freeze = balance_get(user_id, BALANCE_TYPE_FREEZE, asset);
        json_object_set_new(unit, "freeze", json_string(freeze == NULL ? "0" : mpd_to_sci(freeze, 0)));
        json_t *result = json_object();
        json_object_set_new(result, asset, unit);

        return reply_result(ses, pkg, result);
    }

    json_t *result = json_object();
    for (size_t i = 0; i < settings.asset_num; ++i) {
        const char *asset = settings.assets[i].name;
        json_t *unit = json_object();
        mpd_t *available = balance_get(user_id, BALANCE_TYPE_AVAILABLE, asset);
        json_object_set_new(unit, "available", json_string(available == NULL ? "0" : mpd_to_sci(available, 0)));
        mpd_t *freeze = balance_get(user_id, BALANCE_TYPE_FREEZE, asset);
        json_object_set_new(unit, "freeze", json_string(freeze == NULL ? "0" : mpd_to_sci(freeze, 0)));
        json_object_set_new(result, asset, unit);
    }

    return reply_result(ses, pkg, result);
}

static int on_cmd_balance_update(nw_ses *ses, rpc_pkg *pkg, json_t *request)
{
    if (json_array_size(request) != 6)
        return reply_error_invalid_argument(ses, pkg);

    // user_id
    if (!json_is_integer(json_array_get(request, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(request, 0));

    // type
    if (!json_is_integer(json_array_get(request, 1)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t type = json_integer_value(json_array_get(request, 1));
    if (type != BALANCE_TYPE_AVAILABLE && type != BALANCE_TYPE_FREEZE)
        return reply_error_invalid_argument(ses, pkg);

    // asset
    if (!json_is_string(json_array_get(request, 2)))
        return reply_error_invalid_argument(ses, pkg);
    const char *asset = json_string_value(json_array_get(request, 2));
    int prec = asset_exist(asset);
    if (prec < 0)
        return reply_error_invalid_argument(ses, pkg);

    // business
    if (!json_is_string(json_array_get(request, 3)))
        return reply_error_invalid_argument(ses, pkg);
    const char *business = json_string_value(json_array_get(request, 3));

    // business_id
    if (!json_is_integer(json_array_get(request, 4)))
        return reply_error_invalid_argument(ses, pkg);
    uint64_t business_id = json_integer_value(json_array_get(request, 4));

    // change
    if (!json_is_string(json_array_get(request, 5)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *change = decimal(json_string_value(json_array_get(request, 5)), prec);
    if (change == NULL)
        return reply_error_invalid_argument(ses, pkg);

    int ret = update_user_balance(user_id, type, asset, business, business_id, change);
    mpd_del(change);
    if (ret == -1) {
        return reply_error(ses, pkg, 10, "repeat update");
    } else if (ret == -2) {
        return reply_error(ses, pkg, 11, "balance not enough");
    }

    return reply_success(ses, pkg);
}

static void svr_on_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
    json_t *request = json_loadb(pkg->body, pkg->body_size, 0, NULL);
    if (request == NULL) {
        goto decode_error;
    }
    if (!json_is_array(request)) {
        goto decode_error;
    }

    int ret;
    switch (pkg->command) {
    case CMD_BALANCE_QUERY:
        log_trace("from: %s cmd balance query", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_balance_query(ses, pkg, request);
        if (ret < 0) {
            log_error("on_cmd_balance_query fail: %d", ret);
        }
        break;
    case CMD_BALANCE_UPDATE:
        log_trace("from: %s cmd balance update", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_balance_update(ses, pkg, request);
        if (ret < 0) {
            log_error("on_cmd_balance_update fail: %d", ret);
        }
        break;
    default:
        log_error("from: %s unknown command: %u", nw_sock_human_addr(&ses->peer_addr), pkg->command);
        break;
    }

    json_decref(request);
    return;

decode_error:
    if (request) {
        json_decref(request);
    }
    sds hex = hexdump(pkg->body, pkg->body_size);
    log_error("connection: %s, cmd: %u decode request fail, request data: \n%s", \
            nw_sock_human_addr(&ses->peer_addr), pkg->command, hex);
    sdsfree(hex);
    rpc_svr_close_clt(svr, ses);

    return;
}

static void svr_on_new_connection(nw_ses *ses)
{
    log_info("new connection: %s", nw_sock_human_addr(&ses->peer_addr));
}

static void svr_on_connection_close(nw_ses *ses)
{
    log_info("connection: %s close", nw_sock_human_addr(&ses->peer_addr));
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

