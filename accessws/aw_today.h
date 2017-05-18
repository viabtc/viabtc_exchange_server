/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/28, create
 */

# ifndef _AW_TODAY_H_
# define _AW_TODAY_H_

int init_today(void);

int today_subscribe(nw_ses *ses, const char *market);
int today_unsubscribe(nw_ses *ses);
int today_send_last(nw_ses *ses, const char *market);

# endif

