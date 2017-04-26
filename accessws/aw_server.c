/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/21, create
 */

# include "aw_config.h"
# include "aw_server.h"

static ws_svr *svr;
static rpc_clt *listener;

static void on_upgrade(nw_ses *ses)
{
    log_debug("connection: %s upgrade to websocket", nw_sock_human_addr(&ses->peer_addr));
}

static void on_close(nw_ses *ses)
{
    log_debug("websocket connection: %s close", nw_sock_human_addr(&ses->peer_addr));
}

static int on_message(nw_ses *ses, const char *url, void *message, size_t size)
{
    sds msg = sdsnewlen(message, size);
    log_debug("websocket message from: %s, url: %s, message: %s", nw_sock_human_addr(&ses->peer_addr), url, msg);
    ws_send_text(ses, msg);
    sdsfree(msg);
    return -__LINE__;
}

static int init_ws_server(void)
{
    ws_svr_type type;
    memset(&type, 0, sizeof(type));
    type.on_upgrade = on_upgrade;
    type.on_close = on_close;
    type.on_message = on_message;

    svr = ws_svr_create(&settings.svr, &type);
    if (svr == NULL)
        return -__LINE__;

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
    ERR_RET(init_ws_server());
    ERR_RET(init_listener_clt());

    return 0;
}

