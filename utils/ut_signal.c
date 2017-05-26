/*
 * Description: server signal handle
 *     History: yang@haipo.me, 2013/11/30, create
 */

# include <assert.h>
# include <stdlib.h>
# include <string.h>
# include <signal.h>
# include <errno.h>
# include <unistd.h>

# include "ut_log.h"

int signal_exit;
int signal_reload;

struct signal {
    int signo;
    char *signame;
    int flags;
    void (*handler)(int signo);
    int *indicator;
};

static void signal_handler(int signo);

static struct signal signals[] = {
    { SIGUSR1, "SIGUSR1", 0,            signal_handler, NULL, },
    { SIGTTIN, "SIGTTIN", 0,            signal_handler, NULL, },
    { SIGTTOU, "SIGTTOU", 0,            signal_handler, NULL, },
    { SIGQUIT, "SIGQUIT", 0,            signal_handler, NULL, },
    { SIGTERM, "SIGTERM", 0,            signal_handler, NULL, },
    { SIGSEGV, "SIGSEGV", SA_RESETHAND, signal_handler, NULL, },
    { SIGABRT, "SIGABRT", SA_RESETHAND, signal_handler, NULL, },
    { SIGPIPE, "SIGPIPE", 0,            SIG_IGN,        NULL, },
    { SIGCHLD, "SIGCHLD", 0,            SIG_IGN,        NULL, },
    { SIGINT,  "SIGINT",  0,            SIG_IGN,        NULL, },
    { 0      , NULL,      0,            NULL,           NULL, },
};

int init_signal(void)
{
    struct signal *sig;
    for (sig = signals; sig->signo != 0; ++sig) {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = sig->handler;
        sa.sa_flags   = sig->flags;
        sigemptyset(&sa.sa_mask);
        int ret = sigaction(sig->signo, &sa, NULL);
        if (ret < 0) {
            return -1;
        }
    }

    return 0;
}

static void signal_handler(int signo)
{
    struct signal *sig;
    for (sig = signals; sig->signo != 0; ++sig) {
        if (sig->signo == signo)
            break;
    }

    switch (signo) {
    case SIGUSR1:
        log_vip("[%d]signal: %d (%s) received", getpid(), signo, sig->signame);
        signal_reload = 1;
        break;
    case SIGTTIN:
        dlog_level_up();
        log_vip("[%d]signal: %d (%s) received, logging level up: %#x", getpid(), signo, sig->signame, default_dlog_flag);
        break;
    case SIGTTOU:
        dlog_level_down();
        log_vip("[%d]signal: %d (%s) received, logging level down: %#x", getpid(), signo, sig->signame, default_dlog_flag);
        break;
    case SIGQUIT:
        log_vip("[%d]signal: %d (%s) received, exiting", getpid(), signo, sig->signame);
        signal_exit = 1;
        break;
    case SIGTERM:
        log_vip("[%d]signal: %d (%s) received, exiting", getpid(), signo, sig->signame);
        signal_exit = 1;
        break;
    case SIGSEGV:
        log_exception("[%d]signal: %d (%s) received, core dumping", getpid(), signo, sig->signame);
        dlog_flush_all();
        raise(SIGSEGV);
        break;
    case SIGABRT:
        log_exception("[%d]signal: %d (%s) received, core dumping", getpid(), signo, sig->signame);
        dlog_flush_all();
        raise(SIGABRT);
        break;
    default:
        break;
    }
}

