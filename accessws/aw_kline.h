/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/27, create
 */

# ifndef _AW_KLINE_H_
# define _AW_KLINE_H_

int init_kline(void);

int kline_subscribe(nw_ses *ses, const char *market, int interval);
int kline_unsubscribe(nw_ses *ses);

# endif

