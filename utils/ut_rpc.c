/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/30, create
 */

# include <stdlib.h>
# include <assert.h>

# include "ut_rpc.h"
# include "ut_crc32.h"
# include "ut_misc.h"

int rpc_decode(nw_ses *ses, void *data, size_t max)
{
    if (max < RPC_PKG_HEAD_SIZE)
        return 0;

    rpc_pkg *pkg = data;
    if (le32toh(pkg->magic) != RPC_PKG_MAGIC)
        return -1;
    uint32_t pkg_size = RPC_PKG_HEAD_SIZE + le16toh(pkg->ext_size) + le32toh(pkg->body_size);
    if (max < pkg_size)
        return 0;

    uint32_t crc32 = le32toh(pkg->crc32);
    pkg->crc32 = 0;
    if (crc32 != generate_crc32c(data, pkg_size))
        return -3;
    pkg->crc32 = crc32;

    pkg->magic     = le32toh(pkg->magic);
    pkg->command   = le32toh(pkg->command);
    pkg->pkg_type  = le16toh(pkg->pkg_type);
    pkg->result    = le32toh(pkg->result);
    pkg->sequence  = le32toh(pkg->sequence);
    pkg->req_id    = le64toh(pkg->req_id);
    pkg->body_size = le32toh(pkg->body_size);
    pkg->ext_size  = le16toh(pkg->ext_size);

    return pkg_size;
}

int rpc_pack(rpc_pkg *pkg, void **data, uint32_t *size)
{
    static void *send_buf;
    static size_t send_buf_size;
    uint32_t pkg_size;
    if (pkg->body_size > RPC_PKG_MAX_BODY_SIZE) {
        return -1;
    }
    pkg_size = RPC_PKG_HEAD_SIZE + pkg->ext_size + pkg->body_size;
    if (send_buf_size < pkg_size) {
        if (send_buf)
            free(send_buf);
        send_buf_size = pkg_size * 2;
        send_buf = malloc(send_buf_size);
        if (send_buf == NULL) {
            return -1;
        }
    }

    memcpy(send_buf, pkg, RPC_PKG_HEAD_SIZE);
    if (pkg->ext_size)
        memcpy(send_buf + RPC_PKG_HEAD_SIZE, pkg->ext, pkg->ext_size);
    if (pkg->body_size)
        memcpy(send_buf + RPC_PKG_HEAD_SIZE + pkg->ext_size, pkg->body, pkg->body_size);

    pkg = send_buf;
    pkg->magic     = htole32(RPC_PKG_MAGIC);
    pkg->command   = htole32(pkg->command);
    pkg->pkg_type  = htole16(pkg->pkg_type);
    pkg->result    = htole32(pkg->result);
    pkg->sequence  = htole32(pkg->sequence);
    pkg->req_id    = htole64(pkg->req_id);
    pkg->body_size = htole32(pkg->body_size);
    pkg->ext_size  = htole16(pkg->ext_size);

    pkg->crc32 = 0;
    pkg->crc32 = htole32(generate_crc32c(send_buf, pkg_size));

    *data = send_buf;
    *size = pkg_size;

    return 0;
}

int rpc_send(nw_ses *ses, rpc_pkg *pkg)
{
    void *data;
    uint32_t size;
    int ret = rpc_pack(pkg, &data, &size);
    if (ret < 0)
        return ret;
    return nw_ses_send(ses, data, size);
}

