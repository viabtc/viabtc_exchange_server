/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/16, create
 */

# include "me_config.h"
# include "me_server.h"

static rpc_svr *svr;

static int on_cmd_balance_query(nw_ses *ses, rpc_pkg *pkg)
{
    return 0;
}

static int on_cmd_balance_update(nw_ses *ses, rpc_pkg *pkg)
{
    return 0;
}

static void svr_on_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
    int ret;
    switch (pkg->command) {
    case CMD_BALANCE_QUERY:
        log_trace("from: %s cmd balance query", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_balance_query(ses, pkg);
        if (ret < 0) {
            log_error("on_cmd_balance_query fail: %d", ret);
        }
        break;
    case CMD_BALANCE_UPDATE:
        log_trace("from: %s cmd balance update", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_balance_update(ses, pkg);
        if (ret < 0) {
            log_error("on_cmd_balance_update fail: %d", ret);
        }
        break;
    default:
        log_error("from: %s unknown command: %u", nw_sock_human_addr(&ses->peer_addr), pkg->command);
        break;
    }
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

