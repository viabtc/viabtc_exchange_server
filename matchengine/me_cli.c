/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/17, create
 */

# include "me_config.h"
# include "me_balance.h"
# include "me_cli.h"

static cli_svr *svr;

static void on_balance_traverse(uint32_t user_id, uint32_t type, const char *asset, const mpd_t *amount, void *privdata)
{
    sds *reply = privdata;
    *reply = sdscatprintf(*reply, "%-10u %-10u %-10s %s\n", user_id, type, asset, mpd_to_sci(amount, 0));
}

static sds on_cmd_balance_list(const char *cmd, int argc, sds *argv)
{
    sds reply = sdsempty();
    reply = sdscatprintf(reply, "%-10s %-10s %-10s %s\n", "user", "type", "asset", "amount");
    balance_traverse(on_balance_traverse, &reply);
    return reply;
}

static sds on_cmd_balance_get(const char *cmd, int argc, sds *argv)
{
    sds reply = sdsempty();
    if (argc != 2) {
        goto error;
    }

    uint32_t user_id = strtoul(argv[1], NULL, 0);
    for (uint32_t i = 0; i < settings.asset_num; ++i) {
        const char *asset = settings.assets[i].name;
        mpd_t *result = balance_get(user_id, BALANCE_TYPE_AVAILABLE, asset);
        if (result) {
            reply = sdscatprintf(reply, "%-10u %-10u %-10s %s\n", user_id, BALANCE_TYPE_AVAILABLE, asset, mpd_to_sci(result, 0));
        } else {
            reply = sdscatprintf(reply, "%-10u %-10u %-10s %s\n", user_id, BALANCE_TYPE_AVAILABLE, asset, mpd_to_sci(mpd_zero, 0));
        }
        result = balance_get(user_id, BALANCE_TYPE_FREEZE, asset);
        if (result) {
            reply = sdscatprintf(reply, "%-10u %-10u %-10s %s\n", user_id, BALANCE_TYPE_FREEZE, asset, mpd_to_sci(result, 0));
        } else {
            reply = sdscatprintf(reply, "%-10u %-10u %-10s %s\n", user_id, BALANCE_TYPE_FREEZE, asset, mpd_to_sci(mpd_zero, 0));
        }
    }

    return reply;

error:
    return sdsnew("usage: balance get user_id\n");
}

static sds on_cmd_balance(const char *cmd, int argc, sds *argv)
{
    if (argc > 0) {
        if (strcmp(argv[0], "list") == 0) {
            return on_cmd_balance_list(cmd, argc, argv);
        } else if (strcmp(argv[0], "get") == 0) {
            return on_cmd_balance_get(cmd, argc, argv);
        } else {
            goto error;
        }
    }

error:
    return sdsnew("usage: balance list/get\n");
}

int init_cli(void)
{
    svr = cli_svr_create(&settings.cli);
    if (svr == NULL) {
        return -__LINE__;
    }

    cli_svr_add_cmd(svr, "balance", on_cmd_balance);

    return 0;
}

