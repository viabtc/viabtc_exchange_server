/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/30, create
 */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <assert.h>
# include <error.h>
# include <errno.h>

# include "ut_rpc_clt.h"
# include "nw_timer.h"

void on_connect(nw_ses *ses, bool result)
{
    if (result) {
        printf("connect: %s success\n", nw_sock_human_addr(&ses->peer_addr));
    } else {
        printf("connect: %s fail\n", nw_sock_human_addr(&ses->peer_addr));
    }
}

void on_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
}

void on_timer(nw_timer *timer, void *privdata)
{
    char *str = "ping";
    rpc_pkg pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.command = 100;
    pkg.body_size = strlen(str);
    pkg.body = str;
    rpc_clt *clt = privdata;
    rpc_clt_send(clt, &pkg);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("usage: %s name addr...\n", argv[0]);
        exit(0);
    }

    rpc_clt_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.name = strdup(argv[1]);
    cfg.addr_count = argc - 2;
    cfg.addr_arr = malloc(sizeof(nw_addr_t) * cfg.addr_count);
    for (int i = 0; i < cfg.addr_count; ++i) {
        if (nw_sock_cfg_parse(argv[i + 2], &cfg.addr_arr[i], &cfg.sock_type) < 0)
            error(1, errno, "parse peer addr: %s fail", argv[i + 2]);
    }

    rpc_clt_type type;
    memset(&type, 0, sizeof(type));
    type.on_connect = on_connect;
    type.on_recv_pkg = on_recv_pkg;

    rpc_clt *clt = rpc_clt_create(&cfg, &type);
    if (clt == NULL)
        error(1, errno, "rpc_clt_create fail");
    rpc_clt_start(clt);

    nw_timer timer;
    nw_timer_set(&timer, 1.0, true, on_timer, clt);
    nw_timer_start(&timer);

    nw_loop_run();

    return 0;
}

