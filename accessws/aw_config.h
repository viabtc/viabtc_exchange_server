/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/21, create
 */

# ifndef _AW_CONFIG_H_
# define _AW_CONFIG_H_

# include <math.h>
# include <stdio.h>
# include <error.h>
# include <errno.h>
# include <ctype.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <assert.h>
# include <inttypes.h>

# include "nw_svr.h"
# include "nw_clt.h"
# include "nw_job.h"
# include "nw_timer.h"
# include "nw_state.h"

# include "ut_log.h"
# include "ut_sds.h"
# include "ut_cli.h"
# include "ut_misc.h"
# include "ut_list.h"
# include "ut_kafka.h"
# include "ut_signal.h"
# include "ut_config.h"
# include "ut_decimal.h"
# include "ut_rpc_clt.h"
# include "ut_rpc_svr.h"
# include "ut_rpc_cmd.h"
# include "ut_ws_svr.h"

# define ASSET_NAME_MAX_LEN     16
# define MARKET_NAME_MAX_LEN    16
# define SOURCE_MAX_LEN         32
# define INTERVAL_MAX_LEN       16

# define AW_LISTENER_BIND   "seqpacket@/tmp/accessws_listener.sock"

typedef struct depth_limit_cfg {
    int     count;
    int     *limit;
} depth_limit_cfg;

typedef struct depth_merge_cfg {
    int     count;
    mpd_t   **limit;
} depth_merge_cfg;

struct settings {
    process_cfg         process;
    log_cfg             log;
    alert_cfg           alert;
    ws_svr_cfg          svr;
    nw_svr_cfg          monitor;
    rpc_clt_cfg         matchengine;
    rpc_clt_cfg         marketprice;
    rpc_clt_cfg         readhistory;
    kafka_consumer_cfg  orders;
    kafka_consumer_cfg  balances;

    int                 worker_num;
    char                *auth_url;
    char                *sign_url;
    double              backend_timeout;
    double              cache_timeout;

    double              deals_interval;
    double              price_interval;
    double              state_interval;
    double              today_interval;
    double              kline_interval;
    double              depth_interval;

    depth_limit_cfg     depth_limit;
    depth_merge_cfg     depth_merge;
};

extern struct settings settings;
int init_config(const char *path);

# endif

