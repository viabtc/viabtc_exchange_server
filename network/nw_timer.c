/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/20, create
 */

# include "nw_timer.h"

static void on_timer(struct ev_loop *loop, ev_timer *ev, int events)
{
    struct nw_timer *timer = (struct nw_timer *)ev;
    timer->callback(timer, timer->privdata);
}

void nw_timer_set(nw_timer *timer, double interval, bool repeated, nw_timer_callback callback, void *privdata)
{
    nw_loop_init();
    if (repeated) {
        ev_timer_init(&timer->ev, on_timer, interval, interval);
    } else {
        ev_timer_init(&timer->ev, on_timer, interval, 0);
    }
    timer->loop = nw_default_loop;
    timer->interval = interval;
    timer->callback = callback;
    timer->privdata = privdata;
}

void nw_timer_start(nw_timer *timer)
{
    if (!ev_is_active(&timer->ev)) {
        ev_timer_start(timer->loop, &timer->ev);
    }
}

void nw_timer_stop(nw_timer *timer)
{
    if (ev_is_active(&timer->ev)) {
        ev_timer_stop(timer->loop, &timer->ev);
    }
}

bool nw_timer_active(nw_timer *timer)
{
    if (ev_is_active(&timer->ev)) {
        return true;
    }
    return false;
}

double nw_timer_remaining(nw_timer *timer)
{
    return ev_timer_remaining(timer->loop, &timer->ev);
}

