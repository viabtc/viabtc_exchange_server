/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/16, create
 */

# include "me_config.h"
# include "me_operlog.h"
# include "me_market.h"
# include "me_balance.h"
# include "me_update.h"
# include "me_trade.h"
# include "me_persist.h"
# include "me_history.h"
# include "me_message.h"
# include "me_cli.h"
# include "me_server.h"

const char *__process__ = "matchengine";
const char *__version__ = "0.1.0";

nw_timer cron_timer;

static void on_cron_check(nw_timer *timer, void *data)
{
    dlog_check_all();
    if (signal_exit) {
        nw_loop_break();
        signal_exit = 0;
    }
}

static int init_process(void)
{
    if (settings.process.file_limit) {
        if (set_file_limit(settings.process.file_limit) < 0) {
            return -__LINE__;
        }
    }
    if (settings.process.core_limit) {
        if (set_core_limit(settings.process.core_limit) < 0) {
            return -__LINE__;
        }
    }

    return 0;
}

static int init_log(void)
{
    default_dlog = dlog_init(settings.log.path, settings.log.shift, settings.log.max, settings.log.num, settings.log.keep);
    if (default_dlog == NULL)
        return -__LINE__;
    default_dlog_flag = dlog_read_flag(settings.log.flag);
    if (alert_init(&settings.alert) < 0)
        return -__LINE__;

    return 0;
}

int main(int argc, char *argv[])
{
    printf("process: %s version: %s, compile date: %s %s\n", __process__, __version__, __DATE__, __TIME__);

    if (argc < 2) {
        printf("usage: %s config.json\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (process_exist(__process__) != 0) {
        printf("process: %s exist\n", __process__);
        exit(EXIT_FAILURE);
    }

    int ret;
    ret = init_mpd();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init mpd fail: %d", ret);
    }
    ret = init_config(argv[1]);
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "load config fail: %d", ret);
    }
    ret = init_process();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init process fail: %d", ret);
    }
    ret = init_log();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init log fail: %d", ret);
    }
    ret = init_balance();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init balance fail: %d", ret);
    }
    ret = init_update();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init update fail: %d", ret);
    }
    ret = init_trade();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init trade fail: %d", ret);
    }

    daemon(1, 1);
    process_keepalive();

    ret = init_from_db();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init from db fail: %d", ret);
    }
    ret = init_operlog();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init oper log fail: %d", ret);
    }
    ret = init_history();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init history fail: %d", ret);
    }
    ret = init_message();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init message fail: %d", ret);
    }
    ret = init_persist();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init persist fail: %d", ret);
    }
    ret = init_cli();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init cli fail: %d", ret);
    }
    ret = init_server();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init server fail: %d", ret);
    }

    nw_timer_set(&cron_timer, 0.5, true, on_cron_check, NULL);
    nw_timer_start(&cron_timer);

    log_vip("server start");
    log_stderr("server start");
    nw_loop_run();
    log_vip("server stop");

    fini_message();
    fini_history();
    fini_operlog();

    return 0;
}

