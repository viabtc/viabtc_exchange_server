/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/17, create
 */

# include "me_config.h"
# include "me_cli.h"

static cli_svr *svr;

int init_cli(void)
{
    svr = cli_svr_create(&settings.cli);
    if (svr == NULL) {
        return -__LINE__;
    }

    return 0;
}

