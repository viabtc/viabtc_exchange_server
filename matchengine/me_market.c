/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/16, create
 */

# include "me_config.h"
# include "me_market.h"
# include "me_balance.h"

struct dict_user_key {
    uint32_t    user_id;
};

struct dict_order_key {
    uint64_t    order_id;
};

static uint32_t dict_user_hash_function(const void *key)
{
    const struct dict_user_key *obj = key;
    return obj->user_id;
}

static void *dict_user_key_dup(const void *key)
{
    struct dict_user_key *obj = malloc(sizeof(struct dict_user_key));
    if (obj == NULL)
        return NULL;
    memcpy(obj, key, sizeof(struct dict_user_key));
    return obj;
}

static void dict_user_key_free(void *key)
{
    free(key);
}

static void dict_user_val_free(void *key)
{
    list_release(key);
}

static uint32_t dict_order_hash_function(const void *key)
{
    return dict_generic_hash_function(key, sizeof(struct dict_order_key));
}

static void *dict_order_key_dup(const void *key)
{
    struct dict_order_key *obj = malloc(sizeof(struct dict_order_key));
    if (obj == NULL)
        return NULL;
    memcpy(obj, key, sizeof(struct dict_order_key));
    return obj;
}

static void dict_order_key_free(void *key)
{
    free(key);
}

static int order_compare(const void *value1, const void *value2)
{
    const order_t *order1 = value2;
    const order_t *order2 = value2;

    if (order1->id == order2->id) {
        return 0;
    }
    if (order1->type != order2->type) {
        return 1;
    }

    int cmp;
    if (order1->side == MARKET_ORDER_SIDE_ASK) {
        cmp = mpd_cmp(order2->price, order1->price, &mpd_ctx);
    } else {
        cmp = mpd_cmp(order1->price, order2->price, &mpd_ctx);
    }
    if (cmp != 0) {
        return cmp;
    }

    double time_diff = order1->create_time - order2->create_time;
    if (time_diff < 0) {
        return 1;
    } else if (time_diff > 0) {
        return -1;
    };

    return order1->id < order2->id ? 1 : -1;
}

static int order_same(const void *value1, const void *value2)
{
    const order_t *order1 = value2;
    const order_t *order2 = value2;

    if (order1->id == order2->id) {
        return 0;
    }

    return 1;
}

void order_free(order_t *order)
{
    free(order->market_name);
    mpd_del(order->price);
    mpd_del(order->amount);
    mpd_del(order->fee_rate);
    mpd_del(order->left);
    mpd_del(order->freeze);
    mpd_del(order->deal_stock);
    mpd_del(order->deal_money);
    mpd_del(order->deal_fee);
    free(order);
}

static void order_put(market_t *m, order_t *order)
{
    struct dict_order_key order_key = { .order_id = order->id };
    dict_add(m->orders, &order_key, order);

    struct dict_user_key user_key = { .user_id = order->user_id };
    dict_entry *entry = dict_find(m->users, &user_key);
    if (entry) {
        list_t *order_list = entry->val;
        list_add_node_head(order_list, order);
    } else {
        list_type type;
        memset(&type, 0, sizeof(type));
        type.compare = order_same;
        list_t *order_list = list_create(&type);
        list_add_node_head(order_list, order);
        dict_add(m->users, &user_key, order_list);
    }

    if (order->side == MARKET_ORDER_SIDE_ASK) {
        skiplist_insert(m->asks, order);
        balance_freeze(order->user_id, m->stock, order->left);
        mpd_copy(order->freeze, order->left, &mpd_ctx);
    } else {
        skiplist_insert(m->bids, order);
        mpd_t *result = mpd_new(&mpd_ctx);
        mpd_mul(result, order->price, order->amount, &mpd_ctx);
        balance_freeze(order->user_id, m->money, result);
        mpd_copy(order->freeze, result, &mpd_ctx);
        mpd_del(result);
    }
}

static void order_finish(market_t *m, order_t *order)
{
    struct dict_order_key order_key = { .order_id = order->id };
    dict_delete(m->orders, &order_key);

    struct dict_user_key user_key = { .user_id = order->user_id };
    dict_entry *entry = dict_find(m->users, &user_key);
    if (entry) {
        list_t *order_list = entry->val;
        list_node *node = list_find(order_list, order);
        if (node) {
            list_delete(order_list, node);
        }
    }
        
    if (order->side == MARKET_ORDER_SIDE_ASK) {
        skiplist_node *node = skiplist_find(m->asks, order);
        if (node) {
            skiplist_delete(m->asks, node);
        }
        if (mpd_cmp(order->freeze, mpd_zero, &mpd_ctx) > 0) {
            balance_unfreeze(order->user_id, m->stock, order->freeze);
        }
    } else {
        skiplist_node *node = skiplist_find(m->bids, order);
        if (node) {
            skiplist_delete(m->bids, node);
        }
        if (mpd_cmp(order->freeze, mpd_zero, &mpd_ctx) > 0) {
            balance_unfreeze(order->user_id, m->money, order->freeze);
        }
    }

    order_free(order);
}

market_t *market_create(struct market *conf)
{
    if (asset_exist(conf->stock) < 0 || asset_exist(conf->money) < 0)
        return NULL;
    if (conf->stock_prec * conf->money_prec > asset_prec(conf->money))
        return NULL;
    if (conf->stock_prec * conf->fee_prec > asset_prec(conf->stock))
        return NULL;
    if (conf->money_prec * conf->fee_prec > asset_prec(conf->money))
        return NULL;

    market_t *m = malloc(sizeof(market_t));
    m->name  = strdup(conf->name);
    m->stock = strdup(conf->stock);
    m->money = strdup(conf->money);
    m->stock_prec = conf->stock_prec;
    m->money_prec = conf->money_prec;
    m->fee_prec   = conf->fee_prec;

    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function    = dict_user_hash_function;
    dt.key_dup          = dict_user_key_dup;
    dt.key_destructor   = dict_user_key_free;
    dt.val_destructor   = dict_user_val_free;

    m->users = dict_create(&dt, 1024);
    assert(m->users != NULL);

    memset(&dt, 0, sizeof(dt));
    dt.hash_function    = dict_order_hash_function;
    dt.key_dup          = dict_order_key_dup;
    dt.key_destructor   = dict_order_key_free;

    m->orders = dict_create(&dt, 1024);
    assert(m->orders != NULL);

    skiplist_type lt;
    memset(&lt, 0, sizeof(lt));
    lt.compare          = order_compare;

    m->asks = skiplist_create(&lt);
    m->bids = skiplist_create(&lt);
    assert(m->asks != NULL);
    assert(m->bids != NULL);
    
    return m;
}

int execute_limit_ask_order(market_t *m, order_t *order)
{
    mpd_t *price    = mpd_new(&mpd_ctx);
    mpd_t *amount   = mpd_new(&mpd_ctx);
    mpd_t *deal     = mpd_new(&mpd_ctx);
    mpd_t *ask_fee  = mpd_new(&mpd_ctx);
    mpd_t *bid_fee  = mpd_new(&mpd_ctx);
    mpd_t *result   = mpd_new(&mpd_ctx);

    skiplist_node *node;
    skiplist_iter *iter = skiplist_get_iterator(m->bids);
    while ((node = skiplist_next(iter)) != NULL) {
        if (mpd_cmp(order->amount, mpd_zero, &mpd_ctx) == 0) {
            break;
        }

        order_t *pending = node->value;
        if (mpd_cmp(order->price, pending->price, &mpd_ctx) > 0) {
            break;
        }

        mpd_copy(price, pending->price, &mpd_ctx);
        if (mpd_cmp(order->left, pending->left, &mpd_ctx) < 0) {
            mpd_copy(amount, order->left, &mpd_ctx);
        } else {
            mpd_copy(amount, pending->left, &mpd_ctx);
        }

        mpd_mul(deal, price, amount, &mpd_ctx);
        mpd_mul(ask_fee, deal, order->fee_rate, &mpd_ctx);
        mpd_mul(bid_fee, amount, pending->fee_rate, &mpd_ctx);

        mpd_sub(order->left, order->left, amount, &mpd_ctx);
        mpd_add(order->deal_stock, order->deal_stock, amount, &mpd_ctx);
        mpd_add(order->deal_money, order->deal_money, deal, &mpd_ctx);
        mpd_add(order->deal_fee, order->deal_fee, ask_fee, &mpd_ctx);

        mpd_sub(result, deal, ask_fee, &mpd_ctx);
        balance_add(order->user_id, BALANCE_TYPE_AVAILABLE, m->money, result);
        balance_sub(order->user_id, BALANCE_TYPE_AVAILABLE, m->stock, amount);

        mpd_sub(pending->left, pending->left, amount, &mpd_ctx);
        mpd_add(pending->deal_stock, pending->deal_stock, amount, &mpd_ctx);
        mpd_add(pending->deal_money, pending->deal_money, deal, &mpd_ctx);
        mpd_add(pending->deal_fee, pending->deal_fee, bid_fee, &mpd_ctx);
        mpd_sub(pending->freeze, pending->freeze, deal, &mpd_ctx);

        mpd_sub(result, amount, bid_fee, &mpd_ctx);
        balance_add(pending->user_id, BALANCE_TYPE_AVAILABLE, m->stock, result);
        balance_sub(pending->user_id, BALANCE_TYPE_FREEZE, m->money, deal);

        order->update_time = current_timestamp();
        pending->update_time = current_timestamp();

        if (mpd_cmp(pending->left, mpd_zero, &mpd_ctx) == 0) {
            order_finish(m, pending);
        }
    }
    skiplist_release_iterator(iter);

    mpd_del(amount);
    mpd_del(price);
    mpd_del(deal);
    mpd_del(ask_fee);
    mpd_del(bid_fee);
    mpd_del(result);

    return 0;
}

int execute_limit_bid_order(market_t *m, order_t *order)
{
    mpd_t *price    = mpd_new(&mpd_ctx);
    mpd_t *amount   = mpd_new(&mpd_ctx);
    mpd_t *deal     = mpd_new(&mpd_ctx);
    mpd_t *ask_fee  = mpd_new(&mpd_ctx);
    mpd_t *bid_fee  = mpd_new(&mpd_ctx);
    mpd_t *result   = mpd_new(&mpd_ctx);

    skiplist_node *node;
    skiplist_iter *iter = skiplist_get_iterator(m->asks);
    while ((node = skiplist_next(iter)) != NULL) {
        if (mpd_cmp(order->amount, mpd_zero, &mpd_ctx) == 0) {
            break;
        }

        order_t *pending = node->value;
        if (mpd_cmp(order->price, pending->price, &mpd_ctx) < 0) {
            break;
        }

        mpd_copy(price, pending->price, &mpd_ctx);
        if (mpd_cmp(order->left, pending->left, &mpd_ctx) < 0) {
            mpd_copy(amount, order->left, &mpd_ctx);
        } else {
            mpd_copy(amount, pending->left, &mpd_ctx);
        }

        mpd_mul(deal, price, amount, &mpd_ctx);
        mpd_mul(ask_fee, deal, pending->fee_rate, &mpd_ctx);
        mpd_mul(bid_fee, amount, order->fee_rate, &mpd_ctx);

        mpd_sub(order->left, order->left, amount, &mpd_ctx);
        mpd_add(order->deal_stock, order->deal_stock, amount, &mpd_ctx);
        mpd_add(order->deal_money, order->deal_money, deal, &mpd_ctx);
        mpd_add(order->deal_fee, order->deal_fee, bid_fee, &mpd_ctx);

        mpd_sub(result, amount, bid_fee, &mpd_ctx);
        balance_add(order->user_id, BALANCE_TYPE_AVAILABLE, m->stock, result);
        balance_sub(order->user_id, BALANCE_TYPE_AVAILABLE, m->money, deal);

        mpd_sub(pending->left, pending->left, amount, &mpd_ctx);
        mpd_add(pending->deal_stock, pending->deal_stock, amount, &mpd_ctx);
        mpd_add(pending->deal_money, pending->deal_money, deal, &mpd_ctx);
        mpd_add(pending->deal_fee, pending->deal_fee, ask_fee, &mpd_ctx);
        mpd_sub(pending->freeze, pending->freeze, amount, &mpd_ctx);

        mpd_sub(result, deal, ask_fee, &mpd_ctx);
        balance_add(pending->user_id, BALANCE_TYPE_AVAILABLE, m->money, result);
        balance_sub(pending->user_id, BALANCE_TYPE_FREEZE, m->stock, amount);

        order->update_time = current_timestamp();
        pending->update_time = current_timestamp();

        if (mpd_cmp(pending->left, mpd_zero, &mpd_ctx) == 0) {
            order_finish(m, pending);
        }
    }
    skiplist_release_iterator(iter);

    mpd_del(amount);
    mpd_del(price);
    mpd_del(deal);
    mpd_del(ask_fee);
    mpd_del(bid_fee);
    mpd_del(result);

    return 0;
}

int market_put_limit_order(market_t *m, uint32_t user_id, uint32_t side, mpd_t *amount, mpd_t *price, mpd_t *fee_rate)
{
    mpd_t *real_amount = mpd_new(&mpd_ctx);
    mpd_rescale(real_amount, amount, -m->stock_prec, &mpd_ctx);
    if (mpd_cmp(amount, mpd_zero, &mpd_ctx) <= 0) {
        mpd_del(real_amount);
        return -__LINE__;
    }

    mpd_t *real_price = mpd_new(&mpd_ctx);
    mpd_rescale(real_price, price, -m->money_prec, &mpd_ctx);
    if (mpd_cmp(price, mpd_zero, &mpd_ctx) <= 0) {
        mpd_del(real_price);
        mpd_del(real_amount);
        return -__LINE__;
    }

    mpd_t *real_fee = mpd_new(&mpd_ctx);
    mpd_rescale(real_fee, fee_rate, -m->fee_prec, &mpd_ctx);
    if (mpd_cmp(fee_rate, mpd_zero, &mpd_ctx) || mpd_cmp(fee_rate, mpd_one, &mpd_ctx) >= 0) {
        mpd_del(real_fee);
        mpd_del(real_price);
        mpd_del(real_amount);
        return -__LINE__;
    }

    order_t *order = malloc(sizeof(order_t));
    if (order == NULL) {
        mpd_del(real_fee);
        mpd_del(real_price);
        mpd_del(real_amount);
        return -__LINE__;
    }

    order->id = ++m->id_start;
    order->type         = MARKET_ORDER_TYPE_LIMIT;
    order->side         = side;
    order->create_time  = current_timestamp();
    order->update_time  = current_timestamp();
    order->market_name  = strdup(m->name);
    order->user_id      = user_id;
    order->price        = mpd_new(&mpd_ctx);
    order->amount       = mpd_new(&mpd_ctx);
    order->fee_rate     = mpd_new(&mpd_ctx);
    order->left         = mpd_new(&mpd_ctx);
    order->deal_stock   = mpd_new(&mpd_ctx);
    order->deal_money   = mpd_new(&mpd_ctx);
    order->deal_fee     = mpd_new(&mpd_ctx);

    mpd_copy(order->price, real_price, &mpd_ctx);
    mpd_copy(order->amount, real_amount, &mpd_ctx);
    mpd_copy(order->fee_rate, real_fee, &mpd_ctx);
    mpd_copy(order->left, real_amount, &mpd_ctx);
    mpd_copy(order->deal_stock, mpd_zero, &mpd_ctx);
    mpd_copy(order->deal_money, mpd_zero, &mpd_ctx);
    mpd_copy(order->deal_fee, mpd_zero, &mpd_ctx);

    mpd_del(real_fee);
    mpd_del(real_price);
    mpd_del(real_amount);

    int ret;
    if (side == MARKET_ORDER_SIDE_ASK) {
        ret = execute_limit_ask_order(m, order);
    } else {
        ret = execute_limit_bid_order(m, order);
    }
    if (ret < 0) {
        log_error("execute order: %"PRIu64" fail: %d", order->id, ret);
        order_free(order);
        return -__LINE__;
    }

    if (mpd_cmp(order->left, mpd_zero, &mpd_ctx) == 0) {
        order_free(order);
    } else {
        order_put(m, order);
    }

    return 0;
}

int execute_market_ask_order(market_t *m, order_t *order)
{
    mpd_t *price    = mpd_new(&mpd_ctx);
    mpd_t *amount   = mpd_new(&mpd_ctx);
    mpd_t *deal     = mpd_new(&mpd_ctx);
    mpd_t *ask_fee  = mpd_new(&mpd_ctx);
    mpd_t *bid_fee  = mpd_new(&mpd_ctx);
    mpd_t *result   = mpd_new(&mpd_ctx);

    skiplist_node *node;
    skiplist_iter *iter = skiplist_get_iterator(m->bids);
    while ((node = skiplist_next(iter)) != NULL) {
        if (mpd_cmp(order->amount, mpd_zero, &mpd_ctx) == 0) {
            break;
        }

        order_t *pending = node->value;
        mpd_copy(price, pending->price, &mpd_ctx);
        if (mpd_cmp(order->left, pending->left, &mpd_ctx) < 0) {
            mpd_copy(amount, order->left, &mpd_ctx);
        } else {
            mpd_copy(amount, pending->left, &mpd_ctx);
        }

        mpd_mul(deal, price, amount, &mpd_ctx);
        mpd_mul(ask_fee, deal, order->fee_rate, &mpd_ctx);
        mpd_mul(bid_fee, amount, pending->fee_rate, &mpd_ctx);

        mpd_sub(order->left, order->left, amount, &mpd_ctx);
        mpd_add(order->deal_stock, order->deal_stock, amount, &mpd_ctx);
        mpd_add(order->deal_money, order->deal_money, deal, &mpd_ctx);
        mpd_add(order->deal_fee, order->deal_fee, ask_fee, &mpd_ctx);

        mpd_sub(result, deal, ask_fee, &mpd_ctx);
        balance_add(order->user_id, BALANCE_TYPE_AVAILABLE, m->money, result);
        balance_sub(order->user_id, BALANCE_TYPE_AVAILABLE, m->stock, amount);

        mpd_sub(pending->left, pending->left, amount, &mpd_ctx);
        mpd_add(pending->deal_stock, pending->deal_stock, amount, &mpd_ctx);
        mpd_add(pending->deal_money, pending->deal_money, deal, &mpd_ctx);
        mpd_add(pending->deal_fee, pending->deal_fee, bid_fee, &mpd_ctx);
        mpd_sub(pending->freeze, pending->freeze, deal, &mpd_ctx);

        mpd_sub(result, amount, bid_fee, &mpd_ctx);
        balance_add(pending->user_id, BALANCE_TYPE_AVAILABLE, m->stock, result);
        balance_sub(pending->user_id, BALANCE_TYPE_FREEZE, m->money, deal);

        order->update_time = current_timestamp();
        pending->update_time = current_timestamp();

        if (mpd_cmp(pending->left, mpd_zero, &mpd_ctx) == 0) {
            order_finish(m, pending);
        }
    }
    skiplist_release_iterator(iter);

    mpd_del(amount);
    mpd_del(price);
    mpd_del(deal);
    mpd_del(ask_fee);
    mpd_del(bid_fee);
    mpd_del(result);

    return 0;
}

int execute_market_bid_order(market_t *m, order_t *order)
{
    mpd_t *price    = mpd_new(&mpd_ctx);
    mpd_t *amount   = mpd_new(&mpd_ctx);
    mpd_t *deal     = mpd_new(&mpd_ctx);
    mpd_t *ask_fee  = mpd_new(&mpd_ctx);
    mpd_t *bid_fee  = mpd_new(&mpd_ctx);
    mpd_t *result   = mpd_new(&mpd_ctx);

    skiplist_node *node;
    skiplist_iter *iter = skiplist_get_iterator(m->asks);
    while ((node = skiplist_next(iter)) != NULL) {
        if (mpd_cmp(order->amount, mpd_zero, &mpd_ctx) == 0) {
            break;
        }

        order_t *pending = node->value;
        mpd_copy(price, pending->price, &mpd_ctx);

        mpd_div(result, order->left, price, &mpd_ctx);
        mpd_rescale(amount, result, -m->stock_prec, &mpd_ctx);
        if (mpd_cmp(amount, mpd_zero, &mpd_ctx) == 0) {
            break;
        }
        if (mpd_cmp(amount, pending->left, &mpd_ctx) > 0) {
            mpd_copy(amount, pending->left, &mpd_ctx);
        }

        mpd_mul(deal, price, amount, &mpd_ctx);
        mpd_mul(ask_fee, deal, pending->fee_rate, &mpd_ctx);
        mpd_mul(bid_fee, amount, order->fee_rate, &mpd_ctx);

        mpd_sub(order->left, order->left, deal, &mpd_ctx);
        mpd_add(order->deal_stock, order->deal_stock, amount, &mpd_ctx);
        mpd_add(order->deal_money, order->deal_money, deal, &mpd_ctx);
        mpd_add(order->deal_fee, order->deal_fee, bid_fee, &mpd_ctx);

        mpd_sub(result, amount, bid_fee, &mpd_ctx);
        balance_add(order->user_id, BALANCE_TYPE_AVAILABLE, m->stock, result);
        balance_sub(order->user_id, BALANCE_TYPE_AVAILABLE, m->money, deal);

        mpd_sub(pending->left, pending->left, amount, &mpd_ctx);
        mpd_add(pending->deal_stock, pending->deal_stock, amount, &mpd_ctx);
        mpd_add(pending->deal_money, pending->deal_money, deal, &mpd_ctx);
        mpd_add(pending->deal_fee, pending->deal_fee, ask_fee, &mpd_ctx);
        mpd_sub(pending->freeze, pending->freeze, amount, &mpd_ctx);

        mpd_sub(result, deal, ask_fee, &mpd_ctx);
        balance_add(pending->user_id, BALANCE_TYPE_AVAILABLE, m->money, result);
        balance_sub(pending->user_id, BALANCE_TYPE_FREEZE, m->stock, amount);

        order->update_time = current_timestamp();
        pending->update_time = current_timestamp();

        if (mpd_cmp(pending->left, mpd_zero, &mpd_ctx) == 0) {
            order_finish(m, pending);
        }
    }
    skiplist_release_iterator(iter);

    mpd_del(amount);
    mpd_del(price);
    mpd_del(deal);
    mpd_del(ask_fee);
    mpd_del(bid_fee);
    mpd_del(result);

    return 0;
}

int market_put_market_order(market_t *m, uint32_t user_id, uint32_t side, mpd_t *amount, mpd_t *fee_rate)
{
    mpd_t *real_amount = mpd_new(&mpd_ctx);
    if (side == MARKET_ORDER_SIDE_ASK) {
        mpd_rescale(real_amount, amount, -m->stock_prec, &mpd_ctx);
    } else {
        mpd_rescale(real_amount, amount, -m->money_prec, &mpd_ctx);
    }
    if (mpd_cmp(real_amount, mpd_zero, &mpd_ctx) <= 0) {
        mpd_del(real_amount);
        return -__LINE__;
    }

    mpd_t *real_fee = mpd_new(&mpd_ctx);
    mpd_rescale(real_fee, fee_rate, -m->fee_prec, &mpd_ctx);
    if (mpd_cmp(real_fee, mpd_zero, &mpd_ctx) < 0 || mpd_cmp(real_fee, mpd_one, &mpd_ctx) >= 0) {
        mpd_del(real_fee);
        mpd_del(real_amount);
        return -__LINE__;
    }

    order_t *order = malloc(sizeof(order_t));
    if (order == NULL) {
        mpd_del(real_fee);
        mpd_del(real_amount);
        return -__LINE__;
    }

    order->id = ++m->id_start;
    order->type         = MARKET_ORDER_TYPE_MARKET;
    order->side         = side;
    order->create_time  = current_timestamp();
    order->update_time  = current_timestamp();
    order->market_name  = strdup(m->name);
    order->user_id      = user_id;
    order->price        = mpd_new(&mpd_ctx);
    order->amount       = mpd_new(&mpd_ctx);
    order->fee_rate     = mpd_new(&mpd_ctx);
    order->left         = mpd_new(&mpd_ctx);
    order->deal_stock   = mpd_new(&mpd_ctx);
    order->deal_money   = mpd_new(&mpd_ctx);
    order->deal_fee     = mpd_new(&mpd_ctx);

    mpd_copy(order->amount, real_amount, &mpd_ctx);
    mpd_copy(order->fee_rate, real_fee, &mpd_ctx);
    mpd_copy(order->left, order->amount, &mpd_ctx);
    mpd_copy(order->deal_stock, mpd_zero, &mpd_ctx);
    mpd_copy(order->deal_money, mpd_zero, &mpd_ctx);
    mpd_copy(order->deal_fee, mpd_zero, &mpd_ctx);

    mpd_del(real_amount);
    mpd_del(real_fee);

    int ret;
    if (side == MARKET_ORDER_SIDE_ASK) {
        ret = execute_market_ask_order(m, order);
    } else {
        ret = execute_market_bid_order(m, order);
    }
    if (ret < 0) {
        log_error("execute order: %"PRIu64" fail: %d", order->id, ret);
        order_free(order);
        return -__LINE__;
    }

    order_free(order);
    return 0;
}

order_t *market_get_order(market_t *m, uint64_t order_id)
{
    struct dict_order_key key = { .order_id = order_id };
    dict_entry *entry = dict_find(m->orders, &key);
    if (entry) {
        return entry->val;
    }
    return NULL;
}

list_t *market_get_order_list(market_t *m, uint32_t user_id)
{
    struct dict_user_key key = { .user_id = user_id };
    dict_entry *entry = dict_find(m->users, &key);
    if (entry) {
        return entry->val;
    }
    return NULL;
}

