/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/19, create
 */

# include <stdio.h>
# include <errno.h>
# include <unistd.h>

# include "nw_ses.h"

static void libev_on_read_write_evt(struct ev_loop *loop, ev_io *watcher, int events);
static void libev_on_accept_evt(struct ev_loop *loop, ev_io *watcher, int events);
static void libev_on_connect_evt(struct ev_loop *loop, ev_io *watcher, int events);

static void watch_stop(nw_ses *ses)
{
    if (ev_is_active(&ses->ev)) {
        ev_io_stop(ses->loop, &ses->ev);
    }
}

static void watch_read(nw_ses *ses)
{
    if (ev_is_active(&ses->ev)) {
        ev_io_stop(ses->loop, &ses->ev);
    }
    ev_io_init(&ses->ev, libev_on_read_write_evt, ses->sockfd, EV_READ);
    ev_io_start(ses->loop, &ses->ev);
}

static void watch_read_write(nw_ses *ses)
{
    if (ev_is_active(&ses->ev)) {
        ev_io_stop(ses->loop, &ses->ev);
    }
    ev_io_init(&ses->ev, libev_on_read_write_evt, ses->sockfd, EV_READ | EV_WRITE);
    ev_io_start(ses->loop, &ses->ev);
}

static void watch_accept(nw_ses *ses)
{
    ev_io_init(&ses->ev, libev_on_accept_evt, ses->sockfd, EV_READ);
    ev_io_start(ses->loop, &ses->ev);
}

static void watch_connect(nw_ses *ses)
{
    ev_io_init(&ses->ev, libev_on_connect_evt, ses->sockfd, EV_WRITE);
    ev_io_start(ses->loop, &ses->ev);
}

static int nw_write_stream(nw_ses *ses, const void *data, size_t size)
{
    size_t spos = 0;
    while (spos < size) {
        int ret = write(ses->sockfd, data + spos, size - spos);
        if (ret > 0) {
            spos += ret;
        } else if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        } else {
            break;
        }
    }

    return spos;
}

static int nw_write_packet(nw_ses *ses, const void *data, size_t size)
{
    while (true) {
        struct msghdr msg;
        struct iovec io;

        memset(&msg, 0, sizeof(msg));
        io.iov_base = (void *)data;
        io.iov_len = size;
        msg.msg_iov = &io;
        msg.msg_iovlen = 1;

        int ret = sendmsg(ses->sockfd, &msg, MSG_EOR);
        if (ret < 0 && errno == EINTR) {
            continue;
        } else {
            return ret;
        }
    }
}

static void on_can_read(nw_ses *ses)
{
    if (ses->sockfd < 0)
        return;
    if (ses->read_buf == NULL) {
        ses->read_buf = nw_buf_alloc(ses->pool);
        if (ses->read_buf == NULL) {
            ses->on_error(ses, "no recv buf");
            return;
        }
    }

    switch (ses->sock_type) {
    case SOCK_STREAM:
        {
            while (true) {
                int ret = read(ses->sockfd, ses->read_buf->data + ses->read_buf->wpos, nw_buf_avail(ses->read_buf));
                if (ret < 0) {
                    if (errno == EINTR) {
                        continue;
                    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        break;
                    } else {
                        char errmsg[100];
                        snprintf(errmsg, sizeof(errmsg), "read error: %s", strerror(errno));
                        ses->on_error(ses, errmsg);
                        return;
                    }
                } else if (ret == 0) {
                    ses->on_close(ses);
                    return;
                } else {
                    ses->read_buf->wpos += ret;
                }

                size_t size = 0;
                while ((size = nw_buf_size(ses->read_buf)) > 0) {
                    ret = ses->decode_pkg(ses, ses->read_buf->data + ses->read_buf->rpos, size);
                    if (ret < 0) {
                        char errmsg[100];
                        snprintf(errmsg, sizeof(errmsg), "decode msg error: %d", ret);
                        ses->on_error(ses, errmsg);
                        return;
                    } else if (ret > 0) {
                        ses->on_recv_pkg(ses, ses->read_buf->data + ses->read_buf->rpos, ret);
                        if (!ses->read_buf)
                            return;
                        ses->read_buf->rpos += ret;
                    } else {
                        nw_buf_shift(ses->read_buf);
                        if (ses->read_buf->wpos == ses->read_buf->size) {
                            ses->on_error(ses, "decode msg error");
                            return;
                        }
                        break;
                    }
                }

                nw_buf_shift(ses->read_buf);
            }
            if (nw_buf_size(ses->read_buf) == 0) {
                nw_buf_free(ses->pool, ses->read_buf);
                ses->read_buf = NULL;
            }
        }
        break;
    case SOCK_DGRAM:
        {
            while (true) {
                int ret = recvfrom(ses->sockfd, ses->read_buf->data, ses->read_buf->size, 0, \
                        NW_SOCKADDR(&ses->peer_addr), &ses->peer_addr.addrlen);
                if (ret < 0) {
                    if (errno == EINTR) {
                        continue;
                    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        break;
                    } else {
                        char errmsg[100];
                        snprintf(errmsg, sizeof(errmsg), "recvfrom error: %s", strerror(errno));
                        ses->on_error(ses, errmsg);
                        return;
                    }
                }
                int pkg_size = ret;
                ret = ses->decode_pkg(ses, ses->read_buf->data, pkg_size);
                if (ret < 0) {
                    char errmsg[100];
                    snprintf(errmsg, sizeof(errmsg), "decode msg error: %d", ret);
                    ses->on_error(ses, errmsg);
                    return;
                }
                ses->on_recv_pkg(ses, ses->read_buf->data, ret);
                if (!ses->read_buf)
                    return;
            }
            nw_buf_free(ses->pool, ses->read_buf);
            ses->read_buf = NULL;
        }
        break;
    case SOCK_SEQPACKET:
        {
            while (true) {
                struct msghdr msg;
                struct iovec io;
                char control[CMSG_SPACE(sizeof(int))];

                memset(&msg, 0, sizeof(msg));
                io.iov_base = ses->read_buf->data;
                io.iov_len = ses->read_buf->size;
                msg.msg_iov = &io;
                msg.msg_iovlen = 1;
                msg.msg_control = control;
                msg.msg_controllen = sizeof(control);

                int ret = recvmsg(ses->sockfd, &msg, 0);
                if (ret < 0) {
                    if (errno == EINTR) {
                       continue;
                    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        break;
                    } else {
                        char errmsg[100];
                        snprintf(errmsg, sizeof(errmsg), "recvmsg error: %s", strerror(errno));
                        ses->on_error(ses, errmsg);
                        return;
                    }
                } else if (ret == 0) {
                    ses->on_close(ses);
                    return;
                }

                struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
                if (cmsg != NULL) {
                    if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type==SCM_RIGHTS) {
                        int fd = *(int *)CMSG_DATA(cmsg);
                        ses->on_recv_fd(ses, fd);
                    }
                } else {
                    int pkg_size = ret;
                    ret = ses->decode_pkg(ses, ses->read_buf->data, pkg_size);
                    if (ret < 0) {
                        char errmsg[100];
                        snprintf(errmsg, sizeof(errmsg), "decode msg error: %d", ret);
                        ses->on_error(ses, errmsg);
                        return;
                    }
                    ses->on_recv_pkg(ses, ses->read_buf->data, ret);
                    if (!ses->read_buf)
                        return;
                }
            }
            nw_buf_free(ses->pool, ses->read_buf);
            ses->read_buf = NULL;
        }
        break;
    }
}

static void on_can_write(nw_ses *ses)
{
    if (ses->sockfd < 0)
        return;

    while (ses->write_buf->count > 0) {
        nw_buf *buf = ses->write_buf->head;
        size_t size = nw_buf_size(buf);
        int nwrite = 0;
        if (ses->sock_type == SOCK_STREAM) {
            nwrite = nw_write_stream(ses, buf->data + buf->rpos, size);
        } else {
            nwrite = nw_write_packet(ses, buf->data + buf->rpos, size);
        }
        if (nwrite < size) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if (ses->sock_type == SOCK_STREAM) {
                    buf->rpos += nwrite;
                    break;
                } else {
                    break;
                }
            } else {
                char errmsg[100];
                snprintf(errmsg, sizeof(errmsg), "write error: %s", strerror(errno));
                ses->on_error(ses, errmsg);
                return;
            }
        } else {
            nw_buf_list_shift(ses->write_buf);
        }
    }

    if (ses->write_buf->count == 0) {
        watch_read(ses);
    }
}

static void on_can_accept(nw_ses *ses)
{
    if (ses->sockfd < 0)
        return;

    while (true) {
        nw_addr_t peer_addr;
        memset(&peer_addr, 0, sizeof(peer_addr));
        peer_addr.family = ses->host_addr->family;
        peer_addr.addrlen = ses->host_addr->addrlen;
        int sockfd = accept(ses->sockfd, NW_SOCKADDR(&peer_addr), &peer_addr.addrlen);
        if (sockfd < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                char errmsg[100];
                snprintf(errmsg, sizeof(errmsg), "accept error: %s", strerror(errno));
                ses->on_error(ses, errmsg);
                return;
            }
        } else {
            int ret = ses->on_accept(ses, sockfd, &peer_addr);
            if (ret < 0) {
                close(sockfd);
            }
        }
    }
}

static void on_can_connect(nw_ses *ses)
{
    if (ses->sockfd < 0)
        return;
    errno = nw_sock_errno(ses->sockfd);
    if (errno != 0) {
        ses->on_connect(ses, false);
        return;
    }
    watch_read(ses);
    ses->on_connect(ses, true);
}

static void libev_on_read_write_evt(struct ev_loop *loop, ev_io *watcher, int events)
{
    nw_ses *ses = (nw_ses *)watcher;
    if (events & EV_READ)
        on_can_read(ses);
    if (events & EV_WRITE)
        on_can_write(ses);
}

static void libev_on_accept_evt(struct ev_loop *loop, ev_io *watcher, int events)
{
    nw_ses *ses = (nw_ses *)watcher;
    if (events & EV_READ)
        on_can_accept(ses);
}

static void libev_on_connect_evt(struct ev_loop *loop, ev_io *watcher, int events)
{
    nw_ses *ses = (nw_ses *)watcher;
    watch_stop(ses);
    if (events & EV_WRITE)
        on_can_connect(ses);
}

int nw_ses_bind(nw_ses *ses, nw_addr_t *addr)
{
    if (addr->family == AF_UNIX) {
        unlink(addr->un.sun_path);
    }
    int ret = bind(ses->sockfd, NW_SOCKADDR(addr), addr->addrlen);
    if (ret < 0)
        return ret;
    if (addr->family == AF_UNIX) {
        return nw_sock_set_mode(addr, 0777);
    }
    return 0;
}

int nw_ses_listen(nw_ses *ses, int backlog)
{
    int ret = listen(ses->sockfd, backlog);
    if (ret < 0)
        return -1;
    watch_accept(ses);
    return 0;
}

int nw_ses_connect(nw_ses *ses, nw_addr_t *addr)
{
    int ret = connect(ses->sockfd, NW_SOCKADDR(addr), addr->addrlen);
    if (ret == 0) {
        watch_read(ses);
        ses->on_connect(ses, true);
        return 0;
    }
    if (errno == EINPROGRESS) {
        watch_connect(ses);
    } else {
        ses->on_connect(ses, false);
        return -1;
    }
    return 0;
}

int nw_ses_start(nw_ses *ses)
{
    if (ses->ses_type == NW_SES_TYPE_SERVER && (ses->sock_type == SOCK_STREAM || ses->sock_type == SOCK_SEQPACKET)) {
        return nw_ses_listen(ses, SOMAXCONN);
    } else {
        watch_read(ses);
    }
    return 0;
}

int nw_ses_stop(nw_ses *ses)
{
    watch_stop(ses);
    return 0;
}

int nw_ses_send(nw_ses *ses, const void *data, size_t size)
{
    if (ses->sockfd < 0) {
        return -1;
    }

    if (ses->write_buf->count > 0) {
        size_t nwrite;
        if (ses->sock_type == SOCK_STREAM) {
            nwrite = nw_buf_list_write(ses->write_buf, data, size);
        } else {
            nwrite = nw_buf_list_append(ses->write_buf, data, size);
        }
        if (nwrite != size) {
            ses->on_error(ses, "no send buf");
            return -1;
        }
    } else {
        switch (ses->sock_type) {
        case SOCK_STREAM:
            {
                int nwrite = nw_write_stream(ses, data, size);
                if (nwrite < size) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        if (nw_buf_list_write(ses->write_buf, data + nwrite, size - nwrite) != (size - nwrite)) {
                            ses->on_error(ses, "no send buf");
                            return -1;
                        }
                        watch_read_write(ses);
                    } else {
                        char errmsg[100];
                        snprintf(errmsg, sizeof(errmsg), "write error: %s", strerror(errno));
                        ses->on_error(ses, errmsg);
                        return -1;
                    }
                }
            }
            break;
        case SOCK_DGRAM:
            {
                int ret = sendto(ses->sockfd, data, size, 0, NW_SOCKADDR(&ses->peer_addr), ses->peer_addr.addrlen);
                if (ret < 0) {
                    char errmsg[100];
                    snprintf(errmsg, sizeof(errmsg), "sendto error: %s", strerror(errno));
                    ses->on_error(ses, errmsg);
                    return -1;
                }
            }
            break;
        case SOCK_SEQPACKET:
            {
                int ret = nw_write_packet(ses, data, size);
                if (ret < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        if (nw_buf_list_append(ses->write_buf, data, size) != size) {
                            ses->on_error(ses, "on send buf");
                            return -1;
                        }
                        watch_read_write(ses);
                    } else {
                        char errmsg[100];
                        snprintf(errmsg, sizeof(errmsg), "sendmsg error: %s", strerror(errno));
                        ses->on_error(ses, errmsg);
                        return -1;
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    return 0;
}

int nw_ses_send_fd(nw_ses *ses, int fd)
{
    if (ses->sockfd < 0 || ses->sock_type != SOCK_SEQPACKET) {
        return -1;
    }

    struct msghdr msg;
    struct iovec io;
    char control[CMSG_SPACE(sizeof(int))];

    memset(&msg, 0, sizeof(msg));
    io.iov_base = &fd;
    io.iov_len = sizeof(fd);
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    *((int *)CMSG_DATA(cmsg)) = fd;

    return sendmsg(ses->sockfd, &msg, MSG_EOR);
}

int nw_ses_init(nw_ses *ses, struct ev_loop *loop, nw_buf_pool *pool, uint32_t buf_limit, int ses_type)
{
    memset(ses, 0, sizeof(nw_ses));
    ses->loop = loop;
    ses->ses_type = ses_type;
    ses->pool = pool;
    ses->write_buf = nw_buf_list_create(pool, buf_limit);
    if (ses->write_buf == NULL) {
        return -1;
    }

    return 0;
}

int nw_ses_close(nw_ses *ses)
{
    watch_stop(ses);
    ses->id = 0;
    if (ses->sockfd >= 0) {
        close(ses->sockfd);
        ses->sockfd = -1;
    }
    if (ses->read_buf) {
        nw_buf_free(ses->pool, ses->read_buf);
        ses->read_buf = NULL;
    }
    if (ses->write_buf) {
        while (ses->write_buf->count) {
            nw_buf_list_shift(ses->write_buf);
        }
    }

    return 0;
}

int nw_ses_release(nw_ses *ses)
{
    nw_ses_close(ses);
    if (ses->write_buf) {
        nw_buf_list_release(ses->write_buf);
        ses->write_buf = NULL;
    }

    return 0;
}

