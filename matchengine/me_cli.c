/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/17, create
 */

# include "me_cli.h"
# include "me_config.h"
# include "me_balance.h"

static cli_svr *svr;

static sds on_cmd_balance_list(const char *cmd, int argc, sds *argv)
{
    sds reply = sdsempty();
    reply = sdscatprintf(reply, "%-10s %-10s %-10s %s\n", "user", "asset", "type", "amount");

    dict_iterator *iter = dict_get_iterator(dict_balance);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct balance_key *key = entry->key;
        mpd_t *val = entry->val;
        char *str = mpd_to_sci(val, 0);
        if (key->type == BALANCE_TYPE_AVAILABLE) {
            reply = sdscatprintf(reply, "%-10u %-10s %-10s %s\n", key->user_id, key->asset, "available", str);
        } else {
            reply = sdscatprintf(reply, "%-10u %-10s %-10s %s\n", key->user_id, key->asset, "freeze", str);
        }
        free(str);
    }
    dict_release_iterator(iter);

    return reply;
}

static sds on_cmd_balance_get(const char *cmd, int argc, sds *argv)
{
    if (argc != 2) {
        goto error;
    }

    sds reply = sdsempty();
    reply = sdscatprintf(reply, "%-10s %-10s %-10s %s\n", "user", "asset", "type", "amount");
    uint32_t user_id = strtoul(argv[1], NULL, 0);
    for (uint32_t i = 0; i < settings.asset_num; ++i) {
        const char *asset = settings.assets[i].name;
        mpd_t *result = balance_get(user_id, BALANCE_TYPE_AVAILABLE, asset);
        if (result) {
            char *str = mpd_to_sci(result, 0);
            reply = sdscatprintf(reply, "%-10u %-10s %-10s %s\n", user_id, asset, "available", str);
            free(str);
        }
        result = balance_get(user_id, BALANCE_TYPE_FREEZE, asset);
        if (result) {
            char *str = mpd_to_sci(result, 0);
            reply = sdscatprintf(reply, "%-10u %-10s %-10s %s\n", user_id, asset, "freeze", str);
            free(str);
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

