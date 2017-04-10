/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/08, create
 */

# ifndef _ME_MESSAGE_H_
# define _ME_MESSAGE_H_

# include "me_config.h"
# include "me_market.h"

int init_message(void);
int fini_message(void);

int push_deal_message(double t, const char *market, order_t *ask, order_t *bid,
        mpd_t *price, mpd_t *amount, mpd_t *ask_fee, mpd_t *bid_fee);

bool is_message_block(void);

# endif

