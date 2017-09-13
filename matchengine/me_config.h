/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/16, create
 */

# ifndef _ME_CONFIG_H_
# define _ME_CONFIG_H_

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

# include "ut_log.h"
# include "ut_sds.h"
# include "ut_cli.h"
# include "ut_misc.h"
# include "ut_list.h"
# include "ut_mysql.h"
# include "ut_signal.h"
# include "ut_define.h"
# include "ut_config.h"
# include "ut_decimal.h"
# include "ut_rpc_clt.h"
# include "ut_rpc_svr.h"
# include "ut_rpc_cmd.h"
# include "ut_skiplist.h"

# define ASSET_NAME_MAX_LEN     15
# define BUSINESS_NAME_MAX_LEN  31
# define SOURCE_MAX_LEN         31

# define ORDER_BOOK_MAX_LEN     101
# define ORDER_LIST_MAX_LEN     101

# define MAX_PENDING_OPERLOG    100
# define MAX_PENDING_HISTORY    1000
# define MAX_PENDING_MESSAGE    1000

struct asset {
    char                *name;
    int                 prec_save;
    int                 prec_show;
};

struct market {
    char                *name;
    char                *stock;
    char                *money;
    int                 fee_prec;
    int                 stock_prec;
    int                 money_prec;
    mpd_t               *min_amount;
};

struct settings {
    bool                debug;
    process_cfg         process;
    log_cfg             log;
    alert_cfg           alert;
    rpc_svr_cfg         svr;
    cli_svr_cfg         cli;
    mysql_cfg           db_log;
    mysql_cfg           db_history;

    size_t              asset_num;
    struct asset        *assets;

    size_t              market_num;
    struct market       *markets;

    char                *brokers;
    int                 slice_interval;
    int                 slice_keeptime;
    int                 history_thread;
    double              cache_timeout;
};

extern struct settings settings;

int init_config(const char *path);

# endif

