/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/19, create
 */

# include <syslog.h>
# include <unistd.h>
# include <limits.h>
# include <libgen.h>

# include "nw_evt.h"

struct ev_loop *nw_default_loop;
static int initialized;
static int running;
static ev_timer break_timer;

static void fatal_error(const char *msg)
{
    char exe_path[PATH_MAX];
    ssize_t ret = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    exe_path[ret] = 0;

    openlog(basename(exe_path), LOG_PERROR | LOG_PID, LOG_DAEMON);
    syslog(LOG_ERR, "%s", msg);
    closelog();
}

void nw_loop_init(void)
{
    if (initialized)
        return;

    unsigned int flag = 0;
    flag |= EVFLAG_AUTO;
    flag |= EVFLAG_NOENV;
    flag |= EVFLAG_FORKCHECK;
    nw_default_loop = ev_loop_new(flag);
    ev_set_syserr_cb(fatal_error);
    initialized = 1;
}

void nw_loop_run(void)
{
    if (!initialized)
        return;
    
    running = 1;
    ev_run(nw_default_loop, 0);
    return;
}

void nw_loop_break(void)
{
    if (!initialized)
        return;
    if (!running)
        return;

    ev_break(nw_default_loop, EVBREAK_ALL);
    running = 0;
    return;
}

static void on_break_timer(struct ev_loop *loop, ev_timer *ev, int events)
{
    nw_loop_break();
}

void nw_loop_break_later(double waittime)
{
    if (!initialized)
        return;
    if (!running)
        return;

    if (!ev_is_active(&break_timer)) {
        ev_timer_init(&break_timer, on_break_timer, waittime, 0);
        ev_timer_start(nw_default_loop, &break_timer);
    }
}

