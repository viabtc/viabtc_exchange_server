/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/28, create
 */

# ifndef _AW_DEALS_H_
# define _AW_DEALS_H_

int init_deals(void);

int deals_subscribe(nw_ses *ses, const char *market);
int deals_send_full(nw_ses *ses, const char *market);
int deals_unsubscribe(nw_ses *ses);

# endif

