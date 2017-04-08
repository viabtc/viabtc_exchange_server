/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/06, create
 */

# ifndef _ME_HISTORY_H_
# define _ME_HISTORY_H_

# include "me_market.h"

# define HISTORY_HASH_NUM   1000

int init_history(void);
bool is_history_block(void);

int append_order_history(order_t *order);
int append_order_deal_history(double t, uint64_t ask, uint64_t bid, mpd_t *amount, mpd_t *price, mpd_t *deal, mpd_t *ask_fee, mpd_t *bid_fee);
int append_user_balance_history(double t, uint32_t user_id, const char *asset, const char *business, mpd_t *change, const char *detail);

# endif

