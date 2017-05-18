/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/20, create
 */

# ifndef _NW_SVR_H_
# define _NW_SVR_H_

# include <stdint.h>

# include "nw_buf.h"
# include "nw_evt.h"
# include "nw_ses.h"
# include "nw_sock.h"

/* nw_svr is a server object bind on multi addrs with same data process method */

typedef struct nw_svr_bind {
    /* bind addr */
    nw_addr_t addr;
    /* bind type, SOCK_STREAM or SOCK_DGRAM or SOCK_SEQPACKET */
    int sock_type;
} nw_svr_bind;

typedef struct nw_svr_cfg {
    /* size of bind_arr */
    uint32_t bind_count;
    nw_svr_bind *bind_arr;
    /* max full message size */
    uint32_t max_pkg_size;
    /* nw_svr will keep a nw_buf_list for every stream connection to save
     * the pending send data. the buf_limit is the nw_buf_list limit */
    uint32_t buf_limit;
    /* will call nw_sock_set_recv_buf if not 0 */
    uint32_t read_mem;
    /* will call nw_sock_set_send_buf if not 0 */
    uint32_t write_mem;
} nw_svr_cfg;

typedef struct nw_svr_type {
    /* must
     *
     * for dgram and seqpacket connection, every package is consider as a
     * full message, but for stream connection, there is no boundary for
     * a message, use decode_pkg to determine the full message.
     *
     * return < 0: broken data, connection will be closed,
     * return = 0: don't contain a full message, nothing to do,
     * return > 0: return the size of a full message. */
    int (*decode_pkg)(nw_ses *ses, void *data, size_t max);
    /* optional
     *
     * when accept a new connection for non dgram type svr, the default action
     * is add the connection to the server, you can overwrite this action by
     * set on_accept function. return < 0, sockfd will be close */
    int (*on_accept)(nw_ses *ses, int sockfd, nw_addr_t *peer_addr);
    /* optional
     *
     * called when a new connection is established */
    void (*on_new_connection)(nw_ses *ses);
    /* optional
     *
     * called when a connection is close */
    void (*on_connection_close)(nw_ses *ses);
    /* must
     *
     * called when a full message received, put your business logic here */
    void (*on_recv_pkg)(nw_ses *ses, void *data, size_t size);
    /* optional
     *
     * called when a fd is received, if not set, default action is to close it */
    void (*on_recv_fd)(nw_ses *ses, int fd);
    /* optional
     *
     * called when an error occur, msg is the detail of the error */
    void (*on_error_msg)(nw_ses *ses, const char *msg);
    /* optional
     *
     * if set, the on_privdata_free also should be set.
     * the return value will assign to nw_ses privdata
     * called when a new connection is established */
    void *(*on_privdata_alloc)(void *svr);
    /* optional
     *
     * called when on_privdata_alloc is set and the connection is closed */
    void (*on_privdata_free)(void *svr, void *privdata);
} nw_svr_type;

typedef struct nw_svr {
    uint32_t svr_count;
    nw_ses *svr_list;
    nw_svr_type type;
    nw_buf_pool *buf_pool;
    nw_cache *clt_cache;
    nw_ses *clt_list_head;
    nw_ses *clt_list_tail;
    uint32_t clt_count;
    uint32_t buf_limit;
    uint32_t read_mem;
    uint32_t write_mem;
    uint64_t id_start;
    void *privdata;
} nw_svr;

/* create a server instance, the privdata will assign to nw_svr privdata */
nw_svr *nw_svr_create(nw_svr_cfg *cfg, nw_svr_type *type, void *privdata);
int nw_svr_add_clt_fd(nw_svr *svr, int fd);
int nw_svr_start(nw_svr *svr);
int nw_svr_stop(nw_svr *svr);
void nw_svr_release(nw_svr *svr);
void nw_svr_close_clt(nw_svr *svr, nw_ses *ses);

# endif

