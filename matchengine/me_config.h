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

# include <jansson.h>
# include <mpdecimal.h>

# include "nw_svr.h"
# include "nw_clt.h"
# include "nw_timer.h"

# include "ut_log.h"
# include "ut_sds.h"
# include "ut_cli.h"
# include "ut_misc.h"
# include "ut_config.h"
# include "ut_rpc_clt.h"
# include "ut_rpc_svr.h"
# include "ut_rpc_cmd.h"

struct market {
    char                *name;
    char                *stock;
    char                *money;
};

struct settings {
    process_cfg         process;
    log_cfg             log;
    nw_svr_cfg          svr;

    size_t              asset_num;
    char                **assets;
    size_t              market_num;
    struct market       *markets;
};

extern struct settings settings;
int init_config(const char *path);

extern mpd_context_t mpd_ctx;
int init_mpd(void);

# endif

