/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/16, create
 */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>
# include <netdb.h>
# include <unistd.h>

# include "nw_sock.h"

char *nw_sock_human_addr(nw_addr_t *addr)
{
    static char str[128];
    char ip[INET6_ADDRSTRLEN];

    switch (addr->family) {
    case AF_INET:
        inet_ntop(addr->family, &addr->in.sin_addr, ip, sizeof(ip));
        snprintf(str, sizeof(str), "%s:%u", ip, ntohs(addr->in.sin_port));
        break;
    case AF_INET6:
        inet_ntop(addr->family, &addr->in6.sin6_addr, ip, sizeof(ip));
        snprintf(str, sizeof(str), "%s:%u", ip, ntohs(addr->in6.sin6_port));
        break;
    case AF_UNIX:
        snprintf(str, sizeof(str), "%s:%s", "unix", (addr->un.sun_path));
        break;
    default:
        str[0] = 0;
        break;
    }

    return str;
}

char *nw_sock_human_addr_s(nw_addr_t *addr, char *dest)
{
    char ip[INET6_ADDRSTRLEN];

    switch (addr->family) {
    case AF_INET:
        inet_ntop(addr->family, &addr->in.sin_addr, ip, sizeof(ip));
        snprintf(dest, NW_HUMAN_ADDR_SIZE, "%s:%u", ip, ntohs(addr->in.sin_port));
        break;
    case AF_INET6:
        inet_ntop(addr->family, &addr->in6.sin6_addr, ip, sizeof(ip));
        snprintf(dest, NW_HUMAN_ADDR_SIZE, "%s:%u", ip, ntohs(addr->in6.sin6_port));
        break;
    case AF_UNIX:
        snprintf(dest, NW_HUMAN_ADDR_SIZE, "%s:%s", "unix", (addr->un.sun_path));
        break;
    default:
        dest[0] = 0;
        break;
    }

    return dest;
}

char *nw_sock_ip(nw_addr_t *addr)
{
    static char ip[INET6_ADDRSTRLEN];
    switch (addr->family) {
    case AF_INET:
        inet_ntop(addr->family, &addr->in.sin_addr, ip, sizeof(ip));
        break;
    case AF_INET6:
        inet_ntop(addr->family, &addr->in6.sin6_addr, ip, sizeof(ip));
        break;
    default:
        ip[0] = 0;
        break;
    }
    return ip;
}

char *nw_sock_ip_s(nw_addr_t *addr, char *ip)
{
    switch (addr->family) {
    case AF_INET:
        inet_ntop(addr->family, &addr->in.sin_addr, ip, NW_SOCK_IP_SIZE);
        break;
    case AF_INET6:
        inet_ntop(addr->family, &addr->in6.sin6_addr, ip, NW_SOCK_IP_SIZE);
        break;
    default:
        ip[0] = 0;
        break;
    }
    return ip;
}

int nw_sock_set_mode(nw_addr_t *addr, mode_t mode)
{
    if (addr->family != AF_UNIX)
        return 0;
    return chmod(addr->un.sun_path, mode);
}

int nw_sock_peer_addr(int sockfd, nw_addr_t *addr)
{
    addr->addrlen = sizeof(addr->un);
    if (getpeername(sockfd, NW_SOCKADDR(addr), &addr->addrlen) == 0)
    {
        addr->family = NW_SOCKADDR(addr)->sa_family;
        return 0;
    }
    return -1;
}

int nw_sock_host_addr(int sockfd, nw_addr_t *addr)
{
    addr->addrlen = sizeof(addr->un);
    if (getsockname(sockfd, NW_SOCKADDR(addr), &addr->addrlen) == 0)
    {
        addr->family = NW_SOCKADDR(addr)->sa_family;
        return 0;
    }
    return -1;
}

static int nw_sock_addr_fill_inet(nw_addr_t *addr, const char *host, const char *port)
{
    memset(addr, 0, sizeof(nw_addr_t));
    if (strchr(host, '.') != NULL) {
        addr->in.sin_family = AF_INET;
        if (inet_pton(addr->in.sin_family, host, &addr->in.sin_addr) <= 0) {
            return -1;
        }
        addr->in.sin_port = htons(strtoul(port, NULL, 0));
        addr->family = addr->in.sin_family;
        addr->addrlen = sizeof(addr->in);
    } else {
        addr->in6.sin6_family = AF_INET6;
        if (inet_pton(addr->in6.sin6_family, host, &addr->in6.sin6_addr) <= 0) {
            return -1;
        }
        addr->in6.sin6_port = htons(strtoul(port, NULL, 0));
        addr->family = addr->in6.sin6_family;
        addr->addrlen = sizeof(addr->in6);
    }

    return 0;
}

int nw_sock_addr_fill_unix(nw_addr_t *addr, const char* unix_path)
{
    size_t pathlen = strlen(unix_path);
    if (pathlen >= sizeof(addr->un.sun_path)) {
        return -1;
    }
    addr->un.sun_family = AF_UNIX;
    strcpy(addr->un.sun_path, unix_path);
    addr->family = addr->un.sun_family;
    addr->addrlen = sizeof(addr->un);

    return 0;
}

int nw_sock_cfg_parse(const char *cfg, nw_addr_t *addr, int *sock_type)
{
    char *s = strdup(cfg);
    char *sep = strchr(s, '@');
    if (sep == NULL) {
        free(s);
        return -1;
    }
    *sep = '\0';
    char *type = s;
    char *name = sep + 1;
    int is_inet = 0;

    if (strcasecmp(type, "tcp") == 0) {
        *sock_type = SOCK_STREAM;
        is_inet = 1;
    } else if (strcasecmp(type, "udp") == 0) {
        *sock_type = SOCK_DGRAM;
        is_inet = 1;
    } else if (strcasecmp(type, "stream") == 0) {
        *sock_type = SOCK_STREAM;
    } else if (strcasecmp(type, "dgram") == 0) {
        *sock_type = SOCK_DGRAM;
    } else if (strcasecmp(type, "seqpacket") == 0) {
        *sock_type = SOCK_SEQPACKET;
    } else {
        free(s);
        return -2;
    }

    if (is_inet) {
        sep = strchr(name, ':');
        if (sep == NULL) {
            free(s);
            return -3;
        }
        *sep = '\0';
        char *host = name;
        char *port = sep + 1;
        if (nw_sock_addr_fill_inet(addr, host, port) < 0) {
            free(s);
            return -4;
        }
    } else {
        if (nw_sock_addr_fill_unix(addr, name) < 0) {
            free(s);
            return -5;
        }
    }

    free(s);
    return 0;
}

int nw_sock_errno(int sockfd)
{
    int _errno = 0;
    socklen_t len = sizeof(int);
    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &_errno, &len) == 0)
        return _errno;
    if (errno != 0)
        return errno;
    return EBADFD;
}

int nw_sock_get_send_buf(int sockfd, int *buf_size)
{
    socklen_t len = sizeof(*buf_size);
    if (getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, buf_size, &len) != 0)
        return -1;
    return 0;
}

int nw_sock_get_recv_buf(int sockfd, int *buf_size)
{
    socklen_t len = sizeof(*buf_size);
    if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, buf_size, &len) != 0)
        return -1;
    return 0;
}

int nw_sock_set_send_buf(int sockfd, int buf_size)
{
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &buf_size, sizeof(buf_size)) != 0)
        return -1;
    return 0;
}

int nw_sock_set_recv_buf(int sockfd, int buf_size)
{
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size)) != 0)
        return -1;
    return 0;
}

int nw_sock_set_nonblock(int sockfd)
{
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) != 0)
        return -1;
    return 0;
}

int nw_sock_set_no_delay(int sockfd)
{
    int val = 1;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)) != 0)
        return -1;
    return 0;
}

int nw_sock_set_reuse_addr(int sockfd)
{
    int val = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0)
        return -1;
    return 0;
}

