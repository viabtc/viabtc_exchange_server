/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/31, create
 */

# ifndef _UT_CLI_H_
# define _UT_CLI_H_

# include "nw_svr.h"
# include "ut_sds.h"
# include "ut_dict.h"

typedef struct cli_svr_cfg {
    nw_addr_t addr;
    int sock_type;
} cli_svr_cfg;

typedef struct cli_svr {
    nw_svr *raw_svr;
    dict_t *dict;
} cli_svr;

cli_svr *cli_svr_create(cli_svr_cfg *cfg);
void cli_svr_release(cli_svr *svr);

typedef sds (*on_cli_cmd)(const char *cmd, int argc, sds *argv);
void cli_svr_add_cmd(cli_svr *svr, const char *cmd, on_cli_cmd callback);

# endif

