/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/20, create
 */

# ifndef _NW_TIMER_H_
# define _NW_TIMER_H_

# include <stdbool.h>

# include "nw_evt.h"

struct nw_timer;
typedef void (*nw_timer_callback)(struct nw_timer *timer, void *privdata);

typedef struct nw_timer {
    ev_timer ev;
    struct ev_loop *loop;
    double interval;
    nw_timer_callback callback;
    void *privdata;
} nw_timer;

/*
 * set the timer call the callback after interval seconds.
 * if repeated is true, callback will be called every interval seconds
 * privdata will pass to callback function
 */
void nw_timer_set(nw_timer *timer, double interval, bool repeated, nw_timer_callback callback, void *privdata);
void nw_timer_start(nw_timer *timer);
void nw_timer_stop(nw_timer *timer);
bool nw_timer_active(nw_timer *timer);
double nw_timer_remaining(nw_timer *timer);

# endif

