/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/18, create
 */

# include <stdio.h>
# include <stdlib.h>
# include <error.h>
# include <errno.h>
# include <netdb.h> 
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# include "ut_rpc.h"
# include "ut_sds.h"

int main(int argc, char *argv[])
{
    if (argc != 5) {
        error(1, errno, "usage: %s host port cmd data", argv[0]);
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        error(1, errno, "create socket fail");
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    if (inet_aton(argv[1], &addr.sin_addr) == 0) {
        error(1, errno, "ip error");
    }
    addr.sin_port = htons(atoi(argv[2]));

    rpc_pkg req;
    memset(&req, 0, sizeof(req));
    req.command = strtoul(argv[3], NULL, 0);
    req.pkg_type = RPC_PKG_TYPE_REQUEST;
    req.body = argv[4];
    req.body_size = strlen(argv[4]);

    void *data;
    uint32_t data_size;
    rpc_pack(&req, &data, &data_size);
    int ret = sendto(sockfd, data, data_size, 0, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
        error(1, errno, "send error");

    char buf[1024 * 1024];
    ret = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
    if (ret < 0) {
        error(1, errno, "recvfrom fail");
    }

    ret = rpc_decode(NULL, buf, ret);
    if (ret <= 0) {
        error(1, errno, "decode fail");
    }

    struct rpc_pkg rsp;
    memcpy(&rsp, buf, RPC_PKG_HEAD_SIZE);
    rsp.ext = buf + RPC_PKG_HEAD_SIZE;
    rsp.body = rsp.ext + rsp.ext_size;

    sds reply = sdsnewlen(rsp.body, rsp.body_size);
    printf("%s\n", reply);
    sdsfree(reply);

    return 0;
}

