/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/29, create
 */

# ifndef _UT_RPC_H_
# define _UT_RPC_H_

# include <stdint.h>
# include "nw_ses.h"

# define RPC_PKG_MAGIC 0x70656562

# define RPC_PKG_TYPE_REQUEST 0
# define RPC_PKG_TYPE_REPLY   1
# define RPC_PKG_TYPE_PUSH    2

# pragma pack(1)
typedef struct rpc_pkg {
    uint32_t magic;
    uint32_t command;
    uint16_t pkg_type;
    uint32_t result;
    uint32_t crc32;
    uint32_t sequence;
    uint64_t req_id;
    uint32_t body_size;
    uint16_t ext_size;
    void   * ext;
    void   * body;
} rpc_pkg;
# pragma pack()

# define RPC_PKG_HEAD_SIZE (sizeof(rpc_pkg) - sizeof(void *) * 2)
# define RPC_PKG_MAX_BODY_SIZE ((UINT32_MAX / 2) - UINT16_MAX - sizeof(rpc_pkg))

int rpc_decode(nw_ses *ses, void *data, size_t max);
int rpc_pack(rpc_pkg *pkg, void **data, uint32_t *size);
int rpc_send(nw_ses *ses, rpc_pkg *pkg);

# define RPC_CMD_HEARTBEAT 0

# define RPC_HEARTBEAT_INTERVAL         1.0
# define RPC_HEARTBEAT_TIMEOUT_DEFAULT  3.0
# define RPC_HEARTBEAT_TIMEOUT_MIN      2.0
# define RPC_HEARTBEAT_TIMEOUT_MAX      600

# define RPC_HEARTBEAT_TYPE_TIMEOUT     1

# endif

