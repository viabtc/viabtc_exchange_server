/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/21, create
 */

# include "ut_title.h"
# include "ah_config.h"
# include "ah_server.h"
# include "ah_listener.h"

const char *__process__ = "accesshttp";
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
    process_title_init(argc, argv);

    int ret;
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

    for (int i = 0; i < settings.worker_num; ++i) {
        int pid = fork();
        if (pid < 0) {
            error(EXIT_FAILURE, errno, "fork error");
        } else if (pid == 0) {
            process_title_set("%s_worker_%d", __process__, i);
            daemon(1, 1);
            process_keepalive();
            if (i != 0) {
                dlog_set_no_shift(default_dlog);
            }

            ret = init_server();
            if (ret < 0) {
                error(EXIT_FAILURE, errno, "init server fail: %d", ret);
            }

            goto run;
        }
    }

    process_title_set("%s_listener", __process__);
    daemon(1, 1);
    process_keepalive();

    ret = init_listener();
    if (ret < 0) {
        error(EXIT_FAILURE, errno, "init listener fail: %d", ret);
    }
    dlog_set_no_shift(default_dlog);

run:
    nw_timer_set(&cron_timer, 0.5, true, on_cron_check, NULL);
    nw_timer_start(&cron_timer);

    log_vip("server start");
    log_stderr("server start");
    nw_loop_run();
    log_vip("server stop");

    return 0;
}

