/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/17, create
 */

# ifndef _UT_DECIMAL_H_
# define _UT_DECIMAL_H_

# include <mpdecimal.h>

extern mpd_context_t mpd_ctx;

extern mpd_t *mpd_one;
extern mpd_t *mpd_zero;

int init_mpd(void);
mpd_t *decimal(const char *str, int prec);

# endif

