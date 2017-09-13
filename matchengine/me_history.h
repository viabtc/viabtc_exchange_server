/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/06, create
 */

# ifndef _ME_HISTORY_H_
# define _ME_HISTORY_H_

# include "me_market.h"

int init_history(void);
int fini_history(void);

int append_order_history(order_t *order);
int append_order_deal_history(double t, uint64_t deal_id, order_t *ask, int ask_role, order_t *bid, int bid_role, mpd_t *price, mpd_t *amount, mpd_t *deal, mpd_t *ask_fee, mpd_t *bid_fee);
int append_user_balance_history(double t, uint32_t user_id, const char *asset, const char *business, mpd_t *change, const char *detail);

bool is_history_block(void);
sds history_status(sds reply);

# endif

