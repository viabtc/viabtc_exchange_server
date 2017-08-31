/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/17, create
 */

# include "me_cli.h"
# include "me_config.h"
# include "me_balance.h"
# include "me_market.h"
# include "me_trade.h"
# include "me_persist.h"
# include "me_operlog.h"
# include "me_history.h"
# include "me_message.h"

static cli_svr *svr;

static sds on_cmd_status(const char *cmd, int argc, sds *argv)
{
    sds reply = sdsempty();
    reply = market_status(reply);
    reply = operlog_status(reply);
    reply = history_status(reply);
    reply = message_status(reply);
    return reply;
}

static sds on_cmd_balance_list(const char *cmd, int argc, sds *argv)
{
    char *asset = NULL;
    if (argc == 2) {
        asset = argv[1];
    }

    sds reply = sdsempty();
    reply = sdscatprintf(reply, "%-10s %-16s %-10s %s\n", "user", "asset", "type", "amount");

    dict_iterator *iter = dict_get_iterator(dict_balance);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct balance_key *key = entry->key;
        if (asset && strcmp(key->asset, asset) != 0)
            continue;
        mpd_t *val = entry->val;
        char *str = mpd_to_sci(val, 0);
        if (key->type == BALANCE_TYPE_AVAILABLE) {
            reply = sdscatprintf(reply, "%-10u %-16s %-10s %s\n", key->user_id, key->asset, "available", str);
        } else {
            reply = sdscatprintf(reply, "%-10u %-16s %-10s %s\n", key->user_id, key->asset, "freeze", str);
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
    reply = sdscatprintf(reply, "%-10s %-16s %-10s %s\n", "user", "asset", "type", "amount");
    uint32_t user_id = strtoul(argv[1], NULL, 0);
    for (uint32_t i = 0; i < settings.asset_num; ++i) {
        const char *asset = settings.assets[i].name;
        mpd_t *result = balance_get(user_id, BALANCE_TYPE_AVAILABLE, asset);
        if (result) {
            char *str = mpd_to_sci(result, 0);
            reply = sdscatprintf(reply, "%-10u %-16s %-10s %s\n", user_id, asset, "available", str);
            free(str);
        }
        result = balance_get(user_id, BALANCE_TYPE_FREEZE, asset);
        if (result) {
            char *str = mpd_to_sci(result, 0);
            reply = sdscatprintf(reply, "%-10u %-16s %-10s %s\n", user_id, asset, "freeze", str);
            free(str);
        }
    }

    return reply;

error:
    return sdsnew("usage: balance get user_id\n");
}

static sds on_cmd_balance_summary(const char *cmd, int argc, sds *argv)
{
    sds reply = sdsempty();
    reply = sdscatprintf(reply, "%-16s %-30s %-10s %-30s %-10s %-30s\n", "asset", "total", "available", "available", "freeze", "freeze");

    size_t available_count;
    size_t freeze_count;
    mpd_t *total = mpd_new(&mpd_ctx);
    mpd_t *available = mpd_new(&mpd_ctx);
    mpd_t *freeze = mpd_new(&mpd_ctx);
    for (size_t i = 0; i < settings.asset_num; ++i) {
        balance_status(settings.assets[i].name, total, &available_count, available, &freeze_count, freeze);
        char *total_str = mpd_to_sci(total, 0);
        char *available_str = mpd_to_sci(available, 0);
        char *freeze_str = mpd_to_sci(freeze, 0);
        reply = sdscatprintf(reply, "%-16s %-30s %-10zu %-30s %-10zu %-30s\n", settings.assets[i].name,
                total_str, available_count, available_str, freeze_count, freeze_str);
        free(total_str);
        free(available_str);
        free(freeze_str);
    }
    mpd_del(total);
    mpd_del(available);
    mpd_del(freeze);

    return reply;
}

static sds on_cmd_balance(const char *cmd, int argc, sds *argv)
{
    if (argc > 0) {
        if (strcmp(argv[0], "list") == 0) {
            return on_cmd_balance_list(cmd, argc, argv);
        } else if (strcmp(argv[0], "get") == 0) {
            return on_cmd_balance_get(cmd, argc, argv);
        } else if (strcmp(argv[0], "summary") == 0) {
            return on_cmd_balance_summary(cmd, argc, argv);
        } else {
            goto error;
        }
    }

error:
    return sdsnew("usage: balance list/get/summary\n");
}

static sds on_cmd_market_summary(const char *cmd, int argc, sds *argv)
{
    sds reply = sdsempty();
    reply = sdscatprintf(reply, "%-10s %-10s %-20s %-10s %-20s\n", "market", "ask count", "ask amount", "bid count", "bid amount");

    size_t ask_count;
    size_t bid_count;
    mpd_t *ask_amount = mpd_new(&mpd_ctx);
    mpd_t *bid_amount = mpd_new(&mpd_ctx);
    for (size_t i = 0; i < settings.market_num; ++i) {
        market_t *market = get_market(settings.markets[i].name);
        market_get_status(market, &ask_count, ask_amount, &bid_count, bid_amount);
        char *ask_amount_str = mpd_to_sci(ask_amount, 0);
        char *bid_amount_str = mpd_to_sci(bid_amount, 0);
        reply = sdscatprintf(reply, "%-10s %-10zu %-20s %-10zu %-20s\n", market->name, ask_count, ask_amount_str, bid_count, bid_amount_str);
        free(ask_amount_str);
        free(bid_amount_str);
    }
    mpd_del(ask_amount);
    mpd_del(bid_amount);

    return reply;
}

static sds on_cmd_market(const char *cmd, int argc, sds *argv)
{
    if (argc > 0) {
        if (strcmp(argv[0], "summary") == 0) {
            return on_cmd_market_summary(cmd, argc, argv);
        } else {
            goto error;
        }
    }

error:
    return sdsnew("usage market summary\n");
}

static sds on_cmd_makeslice(const char *cmd, int argc, sds *argv)
{
    time_t now = time(NULL);
    make_slice(now);
    return sdsnew("OK\n");
}

int init_cli(void)
{
    svr = cli_svr_create(&settings.cli);
    if (svr == NULL) {
        return -__LINE__;
    }

    cli_svr_add_cmd(svr, "status", on_cmd_status);
    cli_svr_add_cmd(svr, "balance", on_cmd_balance);
    cli_svr_add_cmd(svr, "market",  on_cmd_market);
    cli_svr_add_cmd(svr, "makeslice", on_cmd_makeslice);

    return 0;
}

