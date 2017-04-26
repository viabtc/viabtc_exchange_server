/*
 * Description: 
 *     History: yang@haipo.me, 2016/04/01, create
 */

# include "aw_config.h"
# include "aw_listener.h"

static nw_svr *listener_svr;
static nw_svr *monitor_svr;
static rpc_svr *worker_svr;

static int listener_decode_pkg(nw_ses *ses, void *data, size_t max)
{
    return max;
}
static void listener_on_recv_pkg(nw_ses *ses, void *data, size_t size)
{
    return;
}
static void listener_on_error_msg(nw_ses *ses, const char *msg)
{
    log_error("listener error, peer: %s, msg: %s", nw_sock_human_addr(&ses->peer_addr), msg);
}
static int listener_on_accept(nw_ses *ses, int sockfd, nw_addr_t *peer_addr)
{
    if (worker_svr->raw_svr->clt_count == 0) {
        log_error("no available worker");
        return -1;
    }
    int worker = rand() % worker_svr->raw_svr->clt_count;
    nw_ses *curr = worker_svr->raw_svr->clt_list_head;
    for (int i = 0; i < worker && curr; ++i) {
        curr = curr->next;
    }
    if (!curr) {
        log_error("choice worker fail");
        return -1;
    }
    if (nw_ses_send_fd(curr, sockfd) < 0) {
        log_error("send sockfd fail: %s", strerror(errno));
        return -1;
    }

    close(sockfd);
    return 0;
}

static int init_listener_svr(void)
{
    nw_svr_type type;
    memset(&type, 0, sizeof(type));
    type.decode_pkg = listener_decode_pkg;
    type.on_accept = listener_on_accept;
    type.on_recv_pkg = listener_on_recv_pkg;
    type.on_error_msg = listener_on_error_msg;

    nw_svr_cfg *cfg = (nw_svr_cfg *)&settings.svr;
    listener_svr = nw_svr_create(cfg, &type, NULL);
    if (listener_svr == NULL)
        return -__LINE__;
    if (nw_svr_start(listener_svr) < 0)
        return -__LINE__;

    return 0;
}

static void worker_on_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
    return;
}
static void worker_on_new_connection(nw_ses *ses)
{
    log_info("new worker connected, current worker number: %u", worker_svr->raw_svr->clt_count);
}
static void worker_on_connection_close(nw_ses *ses)
{
    log_info("worker close, current worker number: %u", worker_svr->raw_svr->clt_count - 1);
}

static int init_worker_svr(void)
{
    rpc_svr_cfg cfg;
    nw_svr_bind bind;
    memset(&cfg, 0, sizeof(cfg));
    cfg.bind_count = 1;
    if (nw_sock_cfg_parse(AW_LISTENER_BIND, &bind.addr, &bind.sock_type) < 0)
        return -__LINE__;
    cfg.bind_arr = &bind;
    cfg.max_pkg_size = 1024;
    cfg.heartbeat_check = true;

    rpc_svr_type type;
    type.on_recv_pkg = worker_on_recv_pkg;
    type.on_new_connection = worker_on_new_connection;
    type.on_connection_close = worker_on_connection_close;

    worker_svr = rpc_svr_create(&cfg, &type);
    if (worker_svr == NULL)
        return -__LINE__;
    if (rpc_svr_start(worker_svr) < 0)
        return -__LINE__;

    return 0;
}

static int monitor_decode_pkg(nw_ses *ses, void *data, size_t max)
{
    return max;
}
static void monitor_on_recv_pkg(nw_ses *ses, void *data, size_t size)
{
    return;
}

static int init_monitor_svr(void)
{
    nw_svr_type type;
    memset(&type, 0, sizeof(type));
    type.decode_pkg = monitor_decode_pkg;
    type.on_recv_pkg = monitor_on_recv_pkg;

    monitor_svr = nw_svr_create(&settings.monitor, &type, NULL);
    if (monitor_svr == NULL)
        return -__LINE__;
    if (nw_svr_start(monitor_svr) < 0)
        return -__LINE__;

    return 0;
}

int init_listener(void)
{
    int ret;
    ret = init_listener_svr();
    if (ret < 0)
        return ret;
    ret = init_worker_svr();
    if (ret < 0)
        return ret;
    ret = init_monitor_svr();
    if (ret < 0)
        return ret;

    return 0;
}

