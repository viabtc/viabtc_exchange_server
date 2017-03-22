/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/16, create
 */

# ifndef _ME_MARKET_H_
# define _ME_MARKET_H_

# include "me_config.h"

# define MARKET_ORDER_TYPE_ASK  0
# define MARKET_ORDER_TYPE_BID  1

typedef struct order_t {
    uint64_t        id;
    double          create_time;
    double          update_time;
    uint32_t        user_id;
    char            *market_name;
    uint32_t        type;
    mpd_t           *price;
    mpd_t           *amount;
    mpd_t           *fee_rate;

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
    dict_t          *users;
    dict_t          *orders;
    skiplist_t      *asks;
    skiplist_t      *bids;

    uint64_t        id_start;
} market_t;

market_t *market_create(struct market *conf);
order_t *market_put_order(market_t *m, uint32_t user_id, \
        uint32_t order_type, mpd_t *amount, mpd_t *price, mpd_t *fee_rate);
order_t *market_get_order(market_t *m, uint64_t id);
list_t *market_get_order_list(market_t *m, uint32_t user_id);

# endif

