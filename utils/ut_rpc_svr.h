/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/29, create
 */

# ifndef _UT_RPC_SVR_H_
# define _UT_RPC_SVR_H_

# include "ut_rpc.h"
# include "nw_svr.h"
# include "nw_buf.h"
# include "nw_timer.h"

typedef struct rpc_svr_cfg {
    uint32_t bind_count;
    nw_svr_bind *bind_arr;
    uint32_t max_pkg_size;
    uint32_t buf_limit;
    uint32_t read_mem;
    uint32_t write_mem;
    bool heartbeat_check;
} rpc_svr_cfg;

typedef struct rpc_svr_type {
    void (*on_recv_pkg)(nw_ses *ses, rpc_pkg *pkg);
    void (*on_recv_fd)(nw_ses *ses, int fd);
    void (*on_new_connection)(nw_ses *ses);
    void (*on_connection_close)(nw_ses *ses);
} rpc_svr_type;

typedef struct rpc_svr {
    nw_svr *raw_svr;
    nw_timer timer;
    nw_cache *privdata_cache;
    bool heartbeat_check;
    void (*on_recv_pkg)(nw_ses *ses, rpc_pkg *pkg);
    void (*on_new_connection)(nw_ses *ses);
} rpc_svr;

rpc_svr *rpc_svr_create(rpc_svr_cfg *cfg, rpc_svr_type *type);
int rpc_svr_start(rpc_svr *svr);
int rpc_svr_stop(rpc_svr *svr);
void rpc_svr_release(rpc_svr *svr);
void rpc_svr_close_clt(rpc_svr *svr, nw_ses *ses);
rpc_svr *rpc_svr_from_ses(nw_ses *ses);

# endif

