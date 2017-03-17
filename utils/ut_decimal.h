/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/17, create
 */

# include <mpdecimal.h>

# ifndef _UT_DECIMAL_H_
# define _UT_DECIMAL_H_

extern mpd_context_t mpd_ctx;
extern mpd_t *mpd_zero;

int init_mpd(void);
mpd_t *quantize(mpd_t *val, int num);

# endif

