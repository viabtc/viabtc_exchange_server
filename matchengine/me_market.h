/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/16, create
 */

# ifndef _ME_MARKET_H_
# define _ME_MARKET_H_

# include "me_config.h"

extern uint64_t order_id_start;

# define MARKET_ORDER_TYPE_LIMIT    1
# define MARKET_ORDER_TYPE_MARKET   2

# define MARKET_ORDER_SIDE_ASK      1
# define MARKET_ORDER_SIDE_BID      2

# define MARKET_ROLE_MAKER          1
# define MARKET_ROLE_TAKER          2

typedef struct order_t {
    uint64_t        id;
    uint32_t        type;
    uint32_t        side;
    double          create_time;
    double          update_time;
    uint32_t        user_id;
    char            *market;
    mpd_t           *price;
    mpd_t           *amount;
    mpd_t           *taker_fee;
    mpd_t           *maker_fee;
    mpd_t           *left;
    mpd_t           *freeze;
    mpd_t           *deal_stock;
    mpd_t           *deal_money;
    mpd_t           *deal_fee;
} order_t;

typedef struct market_t {
    char            *name;
    char            *stock;
    char            *money;

    int             stock_prec;
    int             money_prec;
    int             fee_prec;

    dict_t          *orders;
    dict_t          *users;

    skiplist_t      *asks;
    skiplist_t      *bids;
} market_t;

market_t *market_create(struct market *conf);
int market_get_status(market_t *m, size_t *ask_count, mpd_t *ask_amount, size_t *bid_count, mpd_t *bid_amount);

int market_put_limit_order(bool real, market_t *m, uint32_t user_id, uint32_t side, mpd_t *amount, mpd_t *price, mpd_t *taker_fee, mpd_t *maker_fee);
int market_put_market_order(bool real, market_t *m, uint32_t user_id, uint32_t side, mpd_t *amount, mpd_t *taker_fee);

int market_cancel_order(bool real, market_t *m, order_t *order);
int market_put_order(market_t *m, order_t *order);

json_t *get_order_info(order_t *order);
list_t *market_get_order_list(market_t *m, uint32_t user_id);
order_t *market_get_order(market_t *m, uint64_t id);

# endif

