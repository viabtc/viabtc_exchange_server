/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/24, create
 */

# include "rh_config.h"
# include "rh_server.h"
# include "rh_reader.h"

# define MAX_PENDING_JOB 10

static nw_job *job;
static rpc_svr *svr;

struct job_request {
    nw_ses   *ses;
    rpc_pkg  pkg;
    uint64_t ses_id;
    uint32_t command;
    json_t   *params;
};

struct job_reply {
    int     code;
    sds     message;
    json_t  *result;
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

static int reply_error_internal_error(nw_ses *ses, rpc_pkg *pkg)
{
    return reply_error(ses, pkg, 2, "internal error");
}

static int reply_error_service_unavailable(nw_ses *ses, rpc_pkg *pkg)
{
    return reply_error(ses, pkg, 3, "service unavailable");
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

static void *on_job_init(void)
{
    return mysql_connect(&settings.db_history);
}

static int on_cmd_balance_history(MYSQL *conn, json_t *params, struct job_reply *rsp)
{
    if (json_array_size(params) != 7)
        goto invalid_argument;

    uint32_t user_id = json_integer_value(json_array_get(params, 0));
    if (user_id == 0)
        goto invalid_argument;
    const char *asset = json_string_value(json_array_get(params, 1));
    if (asset == NULL)
        goto invalid_argument;
    const char *business = json_string_value(json_array_get(params, 2));
    if (business == NULL)
        goto invalid_argument;
    uint64_t start_time = json_integer_value(json_array_get(params, 3));
    uint64_t end_time   = json_integer_value(json_array_get(params, 4));
    if (end_time && start_time > end_time)
        goto invalid_argument;
    size_t offset = json_integer_value(json_array_get(params, 5));
    size_t limit  = json_integer_value(json_array_get(params, 6));
    if (limit == 0 || limit > QUERY_LIMIT)
        goto invalid_argument;

    json_t *records = get_user_balance_history(conn, user_id, asset, business, start_time, end_time, offset, limit);
    if (records == NULL) {
        rsp->code = 2;
        rsp->message = sdsnew("internal error");
    }

    json_t *result = json_object();
    json_object_set_new(result, "offset", json_integer(offset));
    json_object_set_new(result, "limit", json_integer(limit));
    json_object_set_new(result, "records", records);
    rsp->result = result;

    return 0;

invalid_argument:
    rsp->code = 1;
    rsp->message = sdsnew("invalid argument");

    return 0;
}

static int on_cmd_order_history(MYSQL *conn, json_t *params, struct job_reply *rsp)
{
    if (json_array_size(params) < 6)
        goto invalid_argument;

    uint32_t user_id = json_integer_value(json_array_get(params, 0));
    if (user_id == 0)
        goto invalid_argument;
    const char *market = json_string_value(json_array_get(params, 1));
    if (market == NULL)
        goto invalid_argument;
    uint64_t start_time = json_integer_value(json_array_get(params, 2));
    uint64_t end_time   = json_integer_value(json_array_get(params, 3));
    if (end_time && start_time > end_time)
        goto invalid_argument;
    size_t offset = json_integer_value(json_array_get(params, 4));
    size_t limit  = json_integer_value(json_array_get(params, 5));
    if (limit == 0 || limit > QUERY_LIMIT)
        goto invalid_argument;
    int side = 0;
    if (json_array_size(params) >= 7) {
        side = json_integer_value(json_array_get(params, 6));
        if (side != 0 && side != MARKET_ORDER_SIDE_ASK && side != MARKET_ORDER_SIDE_BID)
            goto invalid_argument;
    }

    json_t *records = get_user_order_finished(conn, user_id, market, side, start_time, end_time, offset, limit);
    if (records == NULL) {
        rsp->code = 2;
        rsp->message = sdsnew("internal error");
    }

    json_t *result = json_object();
    json_object_set_new(result, "offset", json_integer(offset));
    json_object_set_new(result, "limit", json_integer(limit));
    json_object_set_new(result, "records", records);
    rsp->result = result;

    return 0;

invalid_argument:
    rsp->code = 1;
    rsp->message = sdsnew("invalid argument");

    return 0;
}

static int on_cmd_order_deals(MYSQL *conn, json_t *params, struct job_reply *rsp)
{
    if (json_array_size(params) != 3)
        goto invalid_argument;
    uint64_t order_id = json_integer_value(json_array_get(params, 0));
    if (order_id == 0)
        goto invalid_argument;
    size_t offset = json_integer_value(json_array_get(params, 1));
    size_t limit  = json_integer_value(json_array_get(params, 2));
    if (limit == 0 || limit > QUERY_LIMIT)
        goto invalid_argument;

    json_t *records = get_order_deal_details(conn, order_id, offset, limit);
    if (records == NULL) {
        rsp->code = 2;
        rsp->message = sdsnew("internal error");
    }

    json_t *result = json_object();
    json_object_set_new(result, "offset", json_integer(offset));
    json_object_set_new(result, "limit", json_integer(limit));
    json_object_set_new(result, "records", records);
    rsp->result = result;

    return 0;

invalid_argument:
    rsp->code = 1;
    rsp->message = sdsnew("invalid argument");

    return 0;
}

static int on_cmd_order_detail_finished(MYSQL *conn, json_t *params, struct job_reply *rsp)
{
    if (json_array_size(params) != 1)
        goto invalid_argument;
    uint64_t order_id = json_integer_value(json_array_get(params, 0));
    if (order_id == 0)
        goto invalid_argument;

    rsp->result = get_finished_order_detail(conn, order_id);
    if (rsp->result == NULL) {
        rsp->code = 2;
        rsp->message = sdsnew("internal error");
    }

    return 0;

invalid_argument:
    rsp->code = 1;
    rsp->message = sdsnew("invalid argument");

    return 0;
}

static int on_cmd_market_deals(MYSQL *conn, json_t *params, struct job_reply *rsp)
{
    if (json_array_size(params) != 4)
        goto invalid_argument;

    uint32_t user_id = json_integer_value(json_array_get(params, 0));
    if (user_id == 0)
        goto invalid_argument;
    const char *market = json_string_value(json_array_get(params, 1));
    if (market == NULL)
        goto invalid_argument;
    size_t offset = json_integer_value(json_array_get(params, 2));
    size_t limit  = json_integer_value(json_array_get(params, 3));
    if (limit == 0 || limit > QUERY_LIMIT)
        goto invalid_argument;

    json_t *records = get_market_user_deals(conn, user_id, market, offset, limit);
    if (records == NULL) {
        rsp->code = 2;
        rsp->message = sdsnew("internal error");
    }

    json_t *result = json_object();
    json_object_set_new(result, "offset", json_integer(offset));
    json_object_set_new(result, "limit", json_integer(limit));
    json_object_set_new(result, "records", records);
    rsp->result = result;

    return 0;

invalid_argument:
    rsp->code = 1;
    rsp->message = sdsnew("invalid argument");

    return 0;
}

static void on_job(nw_job_entry *entry, void *privdata)
{
    MYSQL *conn = privdata;
    struct job_request *req = entry->request;
    struct job_reply *rsp = malloc(sizeof(struct job_reply));
    entry->reply = rsp;
    if (rsp == NULL) {
        return;
    }
    memset(rsp, 0, sizeof(struct job_reply));

    int ret;
    switch (req->command) {
    case CMD_BALANCE_HISTORY:
        ret = on_cmd_balance_history(conn, req->params, rsp);
        if (ret < 0) {
            log_error("on_cmd_balance_history fail: %d", ret);
        }
        break;
    case CMD_ORDER_HISTORY:
        ret = on_cmd_order_history(conn, req->params, rsp);
        if (ret < 0) {
            log_error("on_cmd_order_history fail: %d", ret);
        }
        break;
    case CMD_ORDER_DEALS:
        ret = on_cmd_order_deals(conn, req->params, rsp);
        if (ret < 0) {
            log_error("on_cmd_order_deals fail: %d", ret);
        }
        break;
    case CMD_ORDER_DETAIL_FINISHED:
        ret = on_cmd_order_detail_finished(conn, req->params, rsp);
        if (ret < 0) {
            log_error("on_cmd_order_detail_finished fail: %d", ret);
        }
        break;
    case CMD_MARKET_USER_DEALS:
        ret = on_cmd_market_deals(conn, req->params, rsp);
        if (ret < 0) {
            log_error("on_cmd_market_deals fail: %d", ret);
        }
        break;
    default:
        log_error("unkown cmd: %u", req->command);
        break;
    }
}

static void on_job_finish(nw_job_entry *entry)
{
    struct job_request *req = entry->request;
    if (req->ses->id != req->ses_id)
        return;
    if (entry->reply == NULL) {
        reply_error_internal_error(req->ses, &req->pkg);
        return;
    }

    struct job_reply *rsp = entry->reply;
    if (rsp->code != 0) {
        reply_error(req->ses, &req->pkg, rsp->code, rsp->message);
        return;
    }

    if (rsp->result) {
        reply_result(req->ses, &req->pkg, rsp->result);
    }
}

static void on_job_cleanup(nw_job_entry *entry)
{
    struct job_request *req = entry->request;
    json_decref(req->params);
    free(req);
    if (entry->reply) {
        struct job_reply *rsp = entry->reply;
        if (rsp->message)
            sdsfree(rsp->message);
        if (rsp->result)
            json_decref(rsp->result);
        free(rsp);
    }
}

static void on_job_release(void *privdata)
{
    mysql_close(privdata);
}

static void svr_on_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
    json_t *params = json_loadb(pkg->body, pkg->body_size, 0, NULL);
    if (params == NULL || !json_is_array(params)) {
        sds hex = hexdump(pkg->body, pkg->body_size);
        log_error("connection: %s, cmd: %u decode params fail, params data: \n%s", \
                nw_sock_human_addr(&ses->peer_addr), pkg->command, hex);
        sdsfree(hex);
        rpc_svr_close_clt(svr, ses);
        return;
    }

    sds params_str = sdsnewlen(pkg->body, pkg->body_size);
    log_debug("from %s command: %u, params: %s", nw_sock_human_addr(&ses->peer_addr), pkg->command, params_str);
    sdsfree(params_str);

    if (job->request_count >= MAX_PENDING_JOB * settings.worker_num) {
        log_error("pending job: %u, service unavailable", job->request_count);
        reply_error_service_unavailable(ses, pkg);
        json_decref(params);
        return;
    }

    struct job_request *req = malloc(sizeof(struct job_request));
    memset(req, 0, sizeof(struct job_request));
    memcpy(&req->pkg, pkg, sizeof(rpc_pkg));
    req->ses = ses;
    req->ses_id = ses->id;
    req->command = pkg->command;
    req->params = params;
    nw_job_add(job, 0, req);

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
    rpc_svr_type st;
    memset(&st, 0, sizeof(st));
    st.on_recv_pkg = svr_on_recv_pkg;
    st.on_new_connection = svr_on_new_connection;
    st.on_connection_close = svr_on_connection_close;

    svr = rpc_svr_create(&settings.svr, &st);
    if (svr == NULL)
        return -__LINE__;
    if (rpc_svr_start(svr) < 0)
        return -__LINE__;

    nw_job_type jt;
    memset(&jt, 0, sizeof(jt));
    jt.on_init    = on_job_init;
    jt.on_job     = on_job;
    jt.on_finish  = on_job_finish;
    jt.on_cleanup = on_job_cleanup;
    jt.on_release = on_job_release;

    job = nw_job_create(&jt, settings.worker_num);
    if (job == NULL)
        return -__LINE__;

    return 0;
}

