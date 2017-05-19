/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/22, create
 */

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>

# include "nw_clt.h"

static int create_socket(int family, int sock_type)
{
    int sockfd = socket(family, sock_type, 0);
    if (sockfd < 0) {
        return -1;
    }
    if (nw_sock_set_nonblock(sockfd) < 0) {
        close(sockfd);
        return -1;
    }
    if (sock_type == SOCK_STREAM && (family == AF_INET || family == AF_INET6)) {
        if (nw_sock_set_no_delay(sockfd) < 0) {
            close(sockfd);
            return -1;
        }
    }

    return sockfd;
}

static int set_socket_option(nw_clt *clt, int sockfd)
{
    if (clt->read_mem > 0) {
        if (nw_sock_set_recv_buf(sockfd, clt->read_mem) < 0) {
            close(sockfd);
            return -1;
        }
    }
    if (clt->write_mem > 0) {
        if (nw_sock_set_send_buf(sockfd, clt->write_mem) < 0) {
            close(sockfd);
            return -1;
        }
    }

    return 0;
}

static void generate_random_path(char *path, size_t size, char *prefix, char *suffix)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * tv.tv_usec);
    char randname[11];
    for (int i = 0; i < 10; ++i) {
        randname[i] = 'a' + rand() % 26;
    }
    randname[10] = '\0';
    snprintf(path, size, "%s/%s%s%s", P_tmpdir, prefix, randname, suffix);
}

static void on_reconnect_timeout(nw_timer *timer, void *privdata)
{
    nw_clt *clt = (nw_clt *)privdata;
    nw_clt_start(clt);
}

static void reconnect_later(nw_clt *clt)
{
    nw_timer_set(&clt->timer, clt->reconnect_timeout, false, on_reconnect_timeout, clt);
    nw_timer_start(&clt->timer);
}

static void on_connect_timeout(nw_timer *timer, void *privdata)
{
    nw_clt *clt = (nw_clt *)privdata;
    if (!clt->on_connect_called) {
        nw_clt_close(clt);
        nw_clt_start(clt);
    }
}

static void watch_connect(nw_clt *clt)
{
    nw_timer_set(&clt->timer, clt->reconnect_timeout, false, on_connect_timeout, clt);
    nw_timer_start(&clt->timer);
}

static void on_recv_fd(nw_ses *ses, int fd)
{
    close(fd);
}

static int clt_close(nw_clt *clt)
{
    if (nw_timer_active(&clt->timer)) {
        nw_timer_stop(&clt->timer);
    }
    clt->connected = false;
    return nw_ses_close(&clt->ses);
}

static void on_connect(nw_ses *ses, bool result)
{
    nw_clt *clt = (nw_clt *)ses;
    clt->on_connect_called = true;
    if (result) {
        clt->connected = true;
        set_socket_option(clt, clt->ses.sockfd);
        nw_sock_host_addr(ses->sockfd, ses->host_addr);
        if (clt->type.on_connect) {
            clt->type.on_connect(ses, result);
        }
    } else {
        if (clt->type.on_connect) {
            clt->type.on_connect(ses, result);
        }
        int ret = 0;
        if (clt->type.on_close) {
            ret = clt->type.on_close(&clt->ses);
        }
        clt_close(clt);
        if (ret > 0) {
            nw_clt_start(clt);
        } else {
            reconnect_later(clt);
        }
    }
}

static void on_error(nw_ses *ses, const char *msg)
{
    nw_clt *clt = (nw_clt *)ses;
    if (clt->type.on_error_msg) {
        clt->type.on_error_msg(ses, msg);
    }
    if (ses->sock_type == SOCK_DGRAM)
        return;
    int ret = 0;
    if (clt->type.on_close) {
        ret = clt->type.on_close(&clt->ses);
    }
    clt_close(clt);
    if (ret > 0) {
        nw_clt_start(clt);
    } else {
        reconnect_later(clt);
    }
}

static void on_close(nw_ses *ses)
{
    nw_clt *clt = (nw_clt *)ses;
    int ret = 0;
    if (clt->type.on_close) {
        ret = clt->type.on_close(&clt->ses);
    }
    clt_close(clt);
    if (ret > 0) {
        nw_clt_start(clt);
    } else {
        reconnect_later(clt);
    }
}

nw_clt *nw_clt_create(nw_clt_cfg *cfg, nw_clt_type *type, void *privdata)
{
    nw_loop_init();

    if (cfg->max_pkg_size == 0)
        return NULL;
    if (type->decode_pkg == NULL)
        return NULL;
    if (type->on_recv_pkg == NULL)
        return NULL;

    nw_clt *clt = malloc(sizeof(nw_clt));
    memset(clt, 0, sizeof(nw_clt));
    clt->type = *type;
    clt->reconnect_timeout = cfg->reconnect_timeout  == 0 ? 1.0 : cfg->reconnect_timeout;
    if (cfg->buf_pool) {
        clt->custom_buf_pool = true;
        clt->buf_pool = cfg->buf_pool;
    } else {
        clt->custom_buf_pool = false;
        clt->buf_pool = nw_buf_pool_create(cfg->max_pkg_size);
        if (clt->buf_pool == NULL) {
            nw_clt_release(clt);
            return NULL;
        }
    }
    clt->read_mem = cfg->read_mem;
    clt->write_mem = cfg->write_mem;

    nw_addr_t *host_addr = malloc(sizeof(nw_addr_t));
    if (host_addr == NULL) {
        nw_clt_release(clt);
        return NULL;
    }
    memset(host_addr, 0, sizeof(nw_addr_t));
    host_addr->family = cfg->addr.family;
    host_addr->addrlen = cfg->addr.addrlen;

    if (nw_ses_init(&clt->ses, nw_default_loop, clt->buf_pool, cfg->buf_limit, NW_SES_TYPE_CLIENT) < 0) {
        nw_clt_release(clt);
        return NULL;
    }
    memcpy(&clt->ses.peer_addr, &cfg->addr, sizeof(nw_addr_t));
    clt->ses.host_addr   = host_addr;
    clt->ses.sockfd      = -1;
    clt->ses.sock_type   = cfg->sock_type;
    clt->ses.privdata    = privdata;

    clt->ses.decode_pkg  = type->decode_pkg;
    clt->ses.on_recv_pkg = type->on_recv_pkg;
    clt->ses.on_recv_fd  = type->on_recv_fd == NULL ? on_recv_fd : type->on_recv_fd;
    clt->ses.on_connect  = on_connect;
    clt->ses.on_error    = on_error;
    clt->ses.on_close    = on_close;

    return clt;
}

int nw_clt_start(nw_clt *clt)
{
    int sockfd = create_socket(clt->ses.peer_addr.family, clt->ses.sock_type);
    if (sockfd < 0) {
        return -1;
    }
    clt->ses.sockfd = sockfd;
    if (clt->ses.peer_addr.family == AF_UNIX && clt->ses.sock_type == SOCK_DGRAM) {
        clt->ses.host_addr->un.sun_family = AF_UNIX;
        generate_random_path(clt->ses.host_addr->un.sun_path, sizeof(clt->ses.host_addr->un.sun_path), "dgram", ".sock");
        if (nw_ses_bind(&clt->ses, clt->ses.host_addr) < 0) {
            return -1;
        }
    }

    if (clt->ses.sock_type == SOCK_STREAM || clt->ses.sock_type == SOCK_SEQPACKET) {
        clt->connected = false;
        clt->on_connect_called = false;
        int ret = nw_ses_connect(&clt->ses, &clt->ses.peer_addr);
        if (ret < 0) {
            if (clt->type.on_close) {
                ret = clt->type.on_close(&clt->ses);
            }
            clt_close(clt);
            if (ret > 0) {
                nw_clt_start(clt);
            } else {
                reconnect_later(clt);
            }
        }
        if (!clt->on_connect_called) {
            watch_connect(clt);
        }
        return 0;
    } else {
        clt->connected = true;
        set_socket_option(clt, clt->ses.sockfd);
        nw_sock_host_addr(clt->ses.sockfd, clt->ses.host_addr);
        return nw_ses_start(&clt->ses);
    }
}

int nw_clt_close(nw_clt *clt)
{
    if (clt->type.on_close) {
        clt->type.on_close(&clt->ses);
    }
    return clt_close(clt);
}

void nw_clt_release(nw_clt *clt)
{
    nw_ses_release(&clt->ses);
    if (!clt->custom_buf_pool && clt->buf_pool) {
        nw_buf_pool_release(clt->buf_pool);
    }
    free(clt->ses.host_addr);
    free(clt);
}

bool nw_clt_connected(nw_clt *clt)
{
    return clt->connected;
}

