/*
 * Description: 
 *     History: wangzheng@viabtc, 2017/05/11, create
 */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <assert.h>
# include <error.h>
# include <errno.h>

# include "ut_rpc_svr.h"
# include "mw_config.h"

#define MONITOR_SVC_MAGIC (10010)

void on_new_connection(nw_ses *ses)
{
    printf("new connection from: %s\n", nw_sock_human_addr(&ses->peer_addr));
}

void on_connection_close(nw_ses *ses)
{
    printf("connection: %s close\n", nw_sock_human_addr(&ses->peer_addr));
}

void on_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
	static char sz_buf[100] = "I got you";
	printf("got message size:%d\n",pkg->body_size);
	//pkg->body = sz_buf;
	//pkg->body_size = sizeof(sz_buf);
    rpc_send(ses, pkg);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s <config file path>...\n", argv[0]);
        exit(0);
    }

    int ret = init_config(argv[1]);
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "load config fail: %d", ret);
    }

    nw_svr_bind bind_arr ;
    memset(&bind_arr, sizeof(nw_svr_bind), 0);

	ret = nw_sock_cfg_parse(settings.svr.bind, &bind_arr.addr, &bind_arr.sock_type);
	if (ret < 0) {
	    printf("parse bind: %s fail: %d\n", argv[1], ret);
	    exit(0);
	}

    nw_svr_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.bind_count = 1;
    cfg.bind_arr = &bind_arr;

    rpc_svr_type type;
    memset(&type, 0, sizeof(type));
    type.on_recv_pkg = on_recv_pkg;
    type.on_new_connection = on_new_connection;
    type.on_connection_close = on_connection_close;

    rpc_svr *svr = rpc_svr_create(&settings.svr, &type);
    if (svr == NULL)
        error(1, errno, "rpc_svr_create fail");
    rpc_svr_start(svr);

    nw_loop_run();

    return 0;
}

