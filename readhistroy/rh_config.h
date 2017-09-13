/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/24, create
 */

# ifndef _RH_CONFIG_H_
# define _RH_CONFIG_H_

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
# include "ut_mysql.h"
# include "ut_signal.h"
# include "ut_config.h"
# include "ut_define.h"
# include "ut_decimal.h"
# include "ut_rpc_clt.h"
# include "ut_rpc_svr.h"
# include "ut_rpc_cmd.h"

# define QUERY_LIMIT    101

struct settings {
    bool                debug;
    process_cfg         process;
    log_cfg             log;
    alert_cfg           alert;
    rpc_svr_cfg         svr;
    mysql_cfg           db_history;
    int                 worker_num;
};

extern struct settings settings;
int init_config(const char *path);

# endif

