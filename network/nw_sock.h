/*
 * Description: socket related
 *     History: yang@haipo.me, 2016/03/16, create
 */

# ifndef _NW_SOCK_H_
# define _NW_SOCK_H_

# include <sys/types.h>
# include <sys/resource.h>
# include <sys/stat.h>
# include <sys/socket.h>
# include <sys/un.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netinet/tcp.h>

/* nw_addr_t is a abstract addr type, hide the difference between different sock type */
typedef struct nw_addr_t {
    unsigned int family;
    unsigned int addrlen;
    union {
        struct sockaddr_in  in;
        struct sockaddr_in6 in6;
        struct sockaddr_un  un;
    };
} nw_addr_t;

# define NW_SOCKADDR(addr) ((struct sockaddr *)(&(addr)->in))
# define NW_HUMAN_ADDR_SIZE 128
# define NW_SOCK_IP_SIZE    INET6_ADDRSTRLEN

/* convert nw_addr_t addr to a human readable string */
char *nw_sock_human_addr(nw_addr_t *addr);

/* nw_sock_human_addr thead safe version, dest should at least NW_HUMAN_ADDR_SIZE len */
char *nw_sock_human_addr_s(nw_addr_t *addr, char *dest);

/* if addr family is AF_INET or AF_INET6, return ip string, else return empty string */
char *nw_sock_ip(nw_addr_t *addr);

/* nw_sock_ip thread safe version, ip should at least NW_SOCK_IP_SIZE len */
char *nw_sock_ip_s(nw_addr_t *addr, char *ip);

/* set unix socket mode */
int nw_sock_set_mode(nw_addr_t *addr, mode_t mode);

/*
 * input: cfg, format: protocol@address
 * protocol list: TCP, UDP, STREAM, DGRAM, SEQPACKET (case-insensitive)
 * addr type: ip:port or unix path. ip can support ipv6
 * example: tcp@127.0.0.1:3333
 *          dgram@/tmp/test.sock
 *
 * output: addr, sock_type
 * sock_type list: SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET
 */
int nw_sock_cfg_parse(const char *cfg, nw_addr_t *addr, int *sock_type);

/* get sockfd peer addr */
int nw_sock_peer_addr(int sockfd, nw_addr_t *addr);

/* get sockfd host addr */
int nw_sock_host_addr(int sockfd, nw_addr_t *addr);

/* get sockfd errno to detect error */
int nw_sock_errno(int sockfd);

/* get sockfd system send buf size */
int nw_sock_get_send_buf(int sockfd, int *buf_size);

/* get sockfd system recv buf size */
int nw_sock_get_recv_buf(int sockfd, int *buf_size);

/* set sockfd system send buf size */
int nw_sock_set_send_buf(int sockfd, int buf_size);

/* set sockfd system send buf size */
int nw_sock_set_recv_buf(int sockfd, int buf_size);

/* set sockfd as nonblock */
int nw_sock_set_nonblock(int sockfd);

/* set sockfd no delay */
int nw_sock_set_no_delay(int sockfd);

/* set sockfd reuse addr */
int nw_sock_set_reuse_addr(int sockfd);

# endif

