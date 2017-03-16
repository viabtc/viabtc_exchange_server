/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/16, create
 */

# include "me_config.h"
# include "me_server.h"

static rpc_svr *svr;

static void svr_on_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
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

