/*
 * Description: net workd event, base on libev
 *     History: yang@haipo.me, 2016/03/19, create
 */

# ifndef _NW_EVT_H_
# define _NW_EVT_H_

# include "ev.h"

/* the default global loop instance */
extern struct ev_loop *nw_default_loop;

/* initialization the event loop */
void nw_loop_init(void);

/* start event loop */
void nw_loop_run(void);

/* break event loop */
void nw_loop_break(void);

/* break event loop after specail seconds */
void nw_loop_break_later(double waittime);

# endif

