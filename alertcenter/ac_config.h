/*
 * Description: 
 *     History: yang@haipo.me, 2016/04/19, create
 */

# ifndef _AC_CONFIG_H_
# define _AC_CONFIG_H_

# include <stdio.h>
# include <ctype.h>
# include <error.h>
# include <errno.h>
# include <unistd.h>
# include <assert.h>

# include "nw_svr.h"
# include "nw_clt.h"
# include "nw_timer.h"
# include "nw_state.h"
# include "ut_log.h"
# include "ut_sds.h"
# include "ut_cli.h"
# include "ut_misc.h"
# include "ut_pack.h"
# include "ut_redis.h"
# include "ut_config.h"

struct settings {
    process_cfg         process;
    log_cfg             log;
    nw_svr_cfg          svr;
    redis_sentinel_cfg  redis;
};

extern struct settings settings;

int load_config(const char *path);

# endif

