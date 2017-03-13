/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/21, create
 */

# ifndef _NW_CLT_H_
# define _NW_CLT_H_

# include <stdint.h>
# include <stdbool.h>

# include "nw_buf.h"
# include "nw_evt.h"
# include "nw_ses.h"
# include "nw_sock.h"
# include "nw_timer.h"

/* nw_clt is a client object, will keep connection to the special addr */

typedef struct nw_clt_cfg {
    /* the target server addr */
    nw_addr_t addr;
    /* the target server sock type */
    int sock_type;
    /* max full message size */
    uint32_t max_pkg_size;
    /* nw_svr will keep a nw_buf_list for every stream connection to save
     * the pending send data. the buf_limit is the nw_buf_list limit */
    uint32_t buf_limit;
    /* will call nw_sock_set_recv_buf if not 0 */
    uint32_t read_mem;
    /* will call nw_sock_set_send_buf if not 0 */
    uint32_t write_mem;
    /* interval to reconnect when the stream connection is close */
    double reconnect_timeout;
    /* buf factory, if set to NULL, nw_clt will create it */
    nw_buf_pool *buf_pool;
} nw_clt_cfg;

typedef struct nw_clt_type {
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
     * called when the connection is close. if on_close return > 0,
     * nw_clt will try reconnect immediately */
    int (*on_close)(nw_ses *ses);
    /* optional
     *
     * called when the connect is success or fail */
    void (*on_connect)(nw_ses *ses, bool result);
    /* must
     *
     * called when a full message is recieved, put your business logic in here */
    void (*on_recv_pkg)(nw_ses *ses, void *data, size_t size);
    /* optional
     *
     * called when a fd is recieved, it not set, default action is close it */
    void (*on_recv_fd)(nw_ses *ses, int fd);
    /* optional
     *
     * called when a error accur, msg is the detail of the error */
    void (*on_error_msg)(nw_ses *ses, const char *msg);
} nw_clt_type;

typedef struct nw_clt {
    nw_ses ses;
    nw_clt_type type;
    bool custom_buf_pool;
    nw_buf_pool *buf_pool;
    nw_timer timer;
    bool connected;
    bool on_connect_called;
    double reconnect_timeout;
    uint32_t read_mem;
    uint32_t write_mem;
} nw_clt;

/* create a client instance, the privdata will assign to nw_clt privdata */
nw_clt *nw_clt_create(nw_clt_cfg *cfg, nw_clt_type *type, void *privdata);
int nw_clt_start(nw_clt *clt);
int nw_clt_close(nw_clt *clt);
void nw_clt_release(nw_clt *clt);
bool nw_clt_connected(nw_clt *clt);

# endif

