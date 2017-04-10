/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/16, create
 */

# include "me_config.h"
# include "me_market.h"
# include "me_balance.h"
# include "me_history.h"
# include "me_message.h"

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

static int dict_user_key_compare(const void *key1, const void *key2)
{
    const struct dict_user_key *obj1 = key1;
    const struct dict_user_key *obj2 = key2;
    if (obj1->user_id > obj2->user_id) {
        return 1;
    } else if (obj1->user_id == obj2->user_id) {
        return 0;
    } else {
        return -1;
    }
}

static void *dict_user_key_dup(const void *key)
{
    struct dict_user_key *obj = malloc(sizeof(struct dict_user_key));
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

static int dict_order_key_compare(const void *key1, const void *key2)
{
    const struct dict_order_key *obj1 = key1;
    const struct dict_order_key *obj2 = key2;
    if (obj1->order_id > obj2->order_id) {
        return 1;
    } else if (obj1->order_id == obj2->order_id) {
        return 0;
    } else {
        return -1;
    }
}

static void *dict_order_key_dup(const void *key)
{
    struct dict_order_key *obj = malloc(sizeof(struct dict_order_key));
    memcpy(obj, key, sizeof(struct dict_order_key));
    return obj;
}

static void dict_order_key_free(void *key)
{
    free(key);
}

static int order_compare(const void *value1, const void *value2)
{
    const order_t *order1 = value1;
    const order_t *order2 = value2;

    if (order1->id == order2->id) {
        return 0;
    }
    if (order1->type != order2->type) {
        return 1;
    }

    int cmp;
    if (order1->side == MARKET_ORDER_SIDE_ASK) {
        cmp = mpd_cmp(order1->price, order2->price, &mpd_ctx);
    } else {
        cmp = mpd_cmp(order2->price, order1->price, &mpd_ctx);
    }
    if (cmp != 0) {
        return cmp;
    }

    return order1->id > order2->id ? 1 : -1;
}

static int order_equality(const void *value1, const void *value2)
{
    const order_t *order1 = value1;
    const order_t *order2 = value2;

    if (order1->id == order2->id) {
        return 0;
    }

    return 1;
}

static void order_free(order_t *order)
{
    mpd_del(order->price);
    mpd_del(order->amount);
    mpd_del(order->fee);
    mpd_del(order->left);
    mpd_del(order->freeze);
    mpd_del(order->deal_stock);
    mpd_del(order->deal_money);
    mpd_del(order->deal_fee);
    free(order->market);
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
        type.compare = order_equality;
        list_t *order_list = list_create(&type);
        list_add_node_head(order_list, order);
        dict_add(m->users, &user_key, order_list);
    }

    if (order->side == MARKET_ORDER_SIDE_ASK) {
        if (skiplist_insert(m->asks, order) == NULL) {
            log_fatal("skiplist_insert fail, order id: %"PRIu64"", order->id);
        }
        if (balance_freeze(order->user_id, m->stock, order->left) == NULL) {
            log_fatal("balance_freeze fail, order id: %"PRIu64"", order->id);
        }
        mpd_copy(order->freeze, order->left, &mpd_ctx);
    } else {
        if (skiplist_insert(m->bids, order) == NULL) {
            log_fatal("skiplist_insert fail, order id: %"PRIu64"", order->id);
        }
        mpd_t *result = mpd_new(&mpd_ctx);
        mpd_mul(result, order->price, order->amount, &mpd_ctx);
        if (balance_freeze(order->user_id, m->money, result) == NULL) {
            log_fatal("balance_freeze fail, order id: %"PRIu64"", order->id);
        }
        mpd_copy(order->freeze, result, &mpd_ctx);
        mpd_del(result);
    }
}

static int order_finish(bool real, market_t *m, order_t *order)
{
    struct dict_order_key order_key = { .order_id = order->id };
    dict_delete(m->orders, &order_key);

    struct dict_user_key user_key = { .user_id = order->user_id };
    dict_entry *entry = dict_find(m->users, &user_key);
    if (entry) {
        list_t *order_list = entry->val;
        list_node *node = list_find(order_list, order);
        if (node) {
            list_del(order_list, node);
        }
    }

    if (order->side == MARKET_ORDER_SIDE_ASK) {
        skiplist_node *node = skiplist_find(m->asks, order);
        if (node) {
            skiplist_delete(m->asks, node);
        }
        if (mpd_cmp(order->freeze, mpd_zero, &mpd_ctx) > 0) {
            if (balance_unfreeze(order->user_id, m->stock, order->freeze) == NULL) {
                log_fatal("balance_unfreeze fail, order id: %"PRIu64"", order->id);
            }
        }
    } else {
        skiplist_node *node = skiplist_find(m->bids, order);
        if (node) {
            skiplist_delete(m->bids, node);
        }
        if (mpd_cmp(order->freeze, mpd_zero, &mpd_ctx) > 0) {
            if (balance_unfreeze(order->user_id, m->money, order->freeze) == NULL) {
                log_fatal("balance_unfreeze fail, order id: %"PRIu64"", order->id);
            }
        }
    }

    if (real) {
        int ret = append_order_history(order);
        if (ret < 0) {
            log_fatal("append_order_history fail: %d, order: %"PRIu64"", ret, order->id);
        }
    }

    order_free(order);
    return 0;
}

market_t *market_create(struct market *conf)
{
    if (asset_exist(conf->stock) < 0 || asset_exist(conf->money) < 0)
        return NULL;
    if (conf->stock_prec + conf->money_prec > asset_prec(conf->money))
        return NULL;
    if (conf->stock_prec + conf->fee_prec > asset_prec(conf->stock))
        return NULL;
    if (conf->money_prec + conf->fee_prec > asset_prec(conf->money))
        return NULL;

    market_t *m = malloc(sizeof(market_t));
    memset(m, 0, sizeof(market_t));
    m->name             = strdup(conf->name);
    m->stock            = strdup(conf->stock);
    m->money            = strdup(conf->money);
    m->stock_prec       = conf->stock_prec;
    m->money_prec       = conf->money_prec;
    m->fee_prec         = conf->fee_prec;

    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function    = dict_user_hash_function;
    dt.key_compare      = dict_user_key_compare;
    dt.key_dup          = dict_user_key_dup;
    dt.key_destructor   = dict_user_key_free;
    dt.val_destructor   = dict_user_val_free;

    m->users = dict_create(&dt, 1024);
    if (m->users == NULL)
        return NULL;

    memset(&dt, 0, sizeof(dt));
    dt.hash_function    = dict_order_hash_function;
    dt.key_compare      = dict_order_key_compare;
    dt.key_dup          = dict_order_key_dup;
    dt.key_destructor   = dict_order_key_free;

    m->orders = dict_create(&dt, 1024);
    if (m->orders == NULL)
        return NULL;

    skiplist_type lt;
    memset(&lt, 0, sizeof(lt));
    lt.compare          = order_compare;

    m->asks = skiplist_create(&lt);
    m->bids = skiplist_create(&lt);
    if (m->asks == NULL || m->bids == NULL)
        return NULL;

    return m;
}

static inline int json_object_set_new_mpd(json_t *obj, const char *key, mpd_t *value)
{
    char *str = mpd_to_sci(value, 0);
    int ret = json_object_set_new(obj, key, json_string(str));
    free(str);
    return ret;
}

static int append_balance_trade_add(order_t *order, const char *asset, const char *business, mpd_t *change, mpd_t *price, mpd_t *amount)
{
    json_t *detail = json_object();
    json_object_set_new(detail, "i", json_integer(order->id));
    json_object_set_new_mpd(detail, "p", price);
    json_object_set_new_mpd(detail, "m", amount);
    char *detail_str = json_dumps(detail, 0);
    int ret = append_user_balance_history(order->update_time, order->user_id, asset, business, change, detail_str);
    free(detail_str);
    json_decref(detail);
    return ret;
}

static int append_balance_trade_sub(order_t *order, const char *asset, const char *business, mpd_t *change, mpd_t *price, mpd_t *amount)
{
    json_t *detail = json_object();
    json_object_set_new(detail, "i", json_integer(order->id));
    json_object_set_new_mpd(detail, "p", price);
    json_object_set_new_mpd(detail, "m", amount);
    char *detail_str = json_dumps(detail, 0);
    mpd_t *real_change = mpd_new(&mpd_ctx);
    mpd_copy_negate(real_change, change, &mpd_ctx);
    int ret = append_user_balance_history(order->update_time, order->user_id, asset, business, real_change, detail_str);
    mpd_del(real_change);
    free(detail_str);
    json_decref(detail);
    return ret;
}

static int execute_limit_ask_order(bool real, market_t *m, order_t *order)
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
        if (mpd_cmp(order->left, mpd_zero, &mpd_ctx) == 0) {
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
        mpd_mul(ask_fee, deal, order->fee, &mpd_ctx);
        mpd_mul(bid_fee, amount, pending->fee, &mpd_ctx);

        order->update_time = pending->update_time = current_timestamp();
        if (real) {
            append_order_deal_history(order->update_time, order->id, pending->id, amount, price, deal, ask_fee, bid_fee);
            push_deal_message(order->update_time, m->name, order, pending, price, amount, ask_fee, bid_fee);
        }

        mpd_sub(order->left, order->left, amount, &mpd_ctx);
        mpd_add(order->deal_stock, order->deal_stock, amount, &mpd_ctx);
        mpd_add(order->deal_money, order->deal_money, deal, &mpd_ctx);
        mpd_add(order->deal_fee, order->deal_fee, ask_fee, &mpd_ctx);

        balance_add(order->user_id, BALANCE_TYPE_AVAILABLE, m->money, deal);
        if (real) {
            append_balance_trade_add(order, m->money, m->name, deal, price, amount);
        }
        if (mpd_cmp(ask_fee, mpd_zero, &mpd_ctx) > 0) {
            balance_sub(order->user_id, BALANCE_TYPE_AVAILABLE, m->money, ask_fee);
            if (real) {
                char business[100];
                snprintf(business, sizeof(business), "%s_FEE", m->name);
                append_balance_trade_sub(order, m->money, business, ask_fee, price, amount);
            }
        }
        balance_sub(order->user_id, BALANCE_TYPE_AVAILABLE, m->stock, amount);
        if (real) {
            append_balance_trade_sub(order, m->stock, m->name, amount, price, amount);
        }

        mpd_sub(pending->left, pending->left, amount, &mpd_ctx);
        mpd_sub(pending->freeze, pending->freeze, deal, &mpd_ctx);
        mpd_add(pending->deal_stock, pending->deal_stock, amount, &mpd_ctx);
        mpd_add(pending->deal_money, pending->deal_money, deal, &mpd_ctx);
        mpd_add(pending->deal_fee, pending->deal_fee, bid_fee, &mpd_ctx);

        balance_add(pending->user_id, BALANCE_TYPE_AVAILABLE, m->stock, amount);
        if (real) {
            append_balance_trade_add(pending, m->stock, m->name, amount, price, amount);
        }
        if (mpd_cmp(bid_fee, mpd_zero, &mpd_ctx) > 0) {
            balance_sub(pending->user_id, BALANCE_TYPE_AVAILABLE, m->stock, bid_fee);
            if (real) {
                char business[100];
                snprintf(business, sizeof(business), "%s_FEE", m->name);
                append_balance_trade_sub(pending, m->stock, business, bid_fee, price, amount);
            }
        }
        balance_sub(pending->user_id, BALANCE_TYPE_FREEZE, m->money, deal);
        if (real) {
            append_balance_trade_sub(pending, m->money, m->name, deal, price, amount);
        }

        char *str_amount  = mpd_to_sci(amount, 0);
        char *str_price   = mpd_to_sci(price, 0);
        char *str_ask_fee = mpd_to_sci(ask_fee, 0);
        char *str_bid_fee = mpd_to_sci(bid_fee, 0);

        log_debug("market: %s, ask uesr: %u, order: %"PRIu64", bid user: %u, order: %"PRIu64", amount: %s, price: %s, ask fee: %s, bid fee: %s",
                m->name, order->user_id, order->id, pending->user_id, pending->id, str_amount, str_price, str_ask_fee, str_bid_fee);

        free(str_amount);
        free(str_price);
        free(str_ask_fee);
        free(str_bid_fee);

        if (mpd_cmp(pending->left, mpd_zero, &mpd_ctx) == 0) {
            order_finish(real, m, pending);
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

static int execute_limit_bid_order(bool real, market_t *m, order_t *order)
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
        if (mpd_cmp(order->left, mpd_zero, &mpd_ctx) == 0) {
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
        mpd_mul(ask_fee, deal, pending->fee, &mpd_ctx);
        mpd_mul(bid_fee, amount, order->fee, &mpd_ctx);

        order->update_time = pending->update_time = current_timestamp();
        if (real) {
            append_order_deal_history(order->update_time, pending->id, order->id, amount, price, deal, ask_fee, bid_fee);
            push_deal_message(order->update_time, m->name, pending, order, price, amount, ask_fee, bid_fee);
        }

        mpd_sub(order->left, order->left, amount, &mpd_ctx);
        mpd_add(order->deal_stock, order->deal_stock, amount, &mpd_ctx);
        mpd_add(order->deal_money, order->deal_money, deal, &mpd_ctx);
        mpd_add(order->deal_fee, order->deal_fee, bid_fee, &mpd_ctx);

        balance_add(order->user_id, BALANCE_TYPE_AVAILABLE, m->stock, amount);
        if (real) {
            append_balance_trade_add(order, m->stock, m->name, amount, price, amount);
        }
        if (mpd_cmp(bid_fee, mpd_zero, &mpd_ctx) > 0) {
            balance_sub(order->user_id, BALANCE_TYPE_AVAILABLE, m->stock, bid_fee);
            if (real) {
                char business[100];
                snprintf(business, sizeof(business), "%s_FEE", m->name);
                append_balance_trade_sub(order, m->stock, business, bid_fee, price, amount);
            }
        }
        balance_sub(order->user_id, BALANCE_TYPE_AVAILABLE, m->money, deal);
        if (real) {
            append_balance_trade_sub(order, m->money, m->name, deal, price, amount);
        }

        mpd_sub(pending->left, pending->left, amount, &mpd_ctx);
        mpd_sub(pending->freeze, pending->freeze, amount, &mpd_ctx);
        mpd_add(pending->deal_stock, pending->deal_stock, amount, &mpd_ctx);
        mpd_add(pending->deal_money, pending->deal_money, deal, &mpd_ctx);
        mpd_add(pending->deal_fee, pending->deal_fee, ask_fee, &mpd_ctx);

        balance_add(pending->user_id, BALANCE_TYPE_AVAILABLE, m->money, deal);
        if (real) {
            append_balance_trade_add(pending, m->money, m->name, deal, price, amount);
        }
        if (mpd_cmp(ask_fee, mpd_zero, &mpd_ctx) > 0) {
            balance_sub(pending->user_id, BALANCE_TYPE_AVAILABLE, m->money, ask_fee);
            if (real) {
                char business[100];
                snprintf(business, sizeof(business), "%s_FEE", m->name);
                append_balance_trade_sub(pending, m->money, business, ask_fee, price, amount);
            }
        }
        balance_sub(pending->user_id, BALANCE_TYPE_FREEZE, m->stock, amount);
        if (real) {
            append_balance_trade_sub(pending, m->stock, m->name, amount, price, amount);
        }

        char *str_amount  = mpd_to_sci(amount, 0);
        char *str_price   = mpd_to_sci(price, 0);
        char *str_ask_fee = mpd_to_sci(ask_fee, 0);
        char *str_bid_fee = mpd_to_sci(bid_fee, 0);

        log_debug("market: %s, ask uesr: %u, order: %"PRIu64", bid user: %u, order: %"PRIu64", amount: %s, price: %s, ask fee: %s, bid fee: %s",
                m->name, pending->user_id, pending->id, order->user_id, order->id, str_amount, str_price, str_ask_fee, str_bid_fee);

        free(str_amount);
        free(str_price);
        free(str_ask_fee);
        free(str_bid_fee);

        if (mpd_cmp(pending->left, mpd_zero, &mpd_ctx) == 0) {
            order_finish(real, m, pending);
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

int market_put_limit_order(bool real, market_t *m, uint32_t user_id, uint32_t side, mpd_t *amount, mpd_t *price, mpd_t *fee)
{
    if (side == MARKET_ORDER_SIDE_ASK) {
        mpd_t *balance = balance_get(user_id, BALANCE_TYPE_AVAILABLE, m->stock);
        if (!balance || mpd_cmp(balance, amount, &mpd_ctx) < 0) {
            return -1;
        }
    } else {
        mpd_t *balance = balance_get(user_id, BALANCE_TYPE_AVAILABLE, m->money);
        mpd_t *require = mpd_new(&mpd_ctx);
        mpd_mul(require, amount, price, &mpd_ctx);
        if (!balance || mpd_cmp(balance, require, &mpd_ctx) < 0) {
            mpd_del(require);
            return -1;
        }
        mpd_del(require);
    }

    order_t *order = malloc(sizeof(order_t));
    if (order == NULL) {
        return -__LINE__;
    }

    order->id           = ++m->id_start;
    order->type         = MARKET_ORDER_TYPE_LIMIT;
    order->side         = side;
    order->create_time  = current_timestamp();
    order->update_time  = order->create_time;
    order->market       = strdup(m->name);
    order->user_id      = user_id;
    order->price        = mpd_new(&mpd_ctx);
    order->amount       = mpd_new(&mpd_ctx);
    order->fee          = mpd_new(&mpd_ctx);
    order->left         = mpd_new(&mpd_ctx);
    order->freeze       = mpd_new(&mpd_ctx);
    order->deal_stock   = mpd_new(&mpd_ctx);
    order->deal_money   = mpd_new(&mpd_ctx);
    order->deal_fee     = mpd_new(&mpd_ctx);

    mpd_copy(order->price, price, &mpd_ctx);
    mpd_copy(order->amount, amount, &mpd_ctx);
    mpd_copy(order->fee, fee, &mpd_ctx);
    mpd_copy(order->left, amount, &mpd_ctx);
    mpd_copy(order->deal_stock, mpd_zero, &mpd_ctx);
    mpd_copy(order->deal_money, mpd_zero, &mpd_ctx);
    mpd_copy(order->deal_fee, mpd_zero, &mpd_ctx);

    int ret;
    if (side == MARKET_ORDER_SIDE_ASK) {
        ret = execute_limit_ask_order(real, m, order);
    } else {
        ret = execute_limit_bid_order(real, m, order);
    }
    if (ret < 0) {
        log_error("execute order: %"PRIu64" fail: %d", order->id, ret);
        order_free(order);
        return -__LINE__;
    }

    if (mpd_cmp(order->left, mpd_zero, &mpd_ctx) == 0) {
        if (real) {
            int ret = append_order_history(order);
            if (ret < 0) {
                log_fatal("append_order_history fail: %d, order: %"PRIu64"", ret, order->id);
            }
        }
        order_free(order);
    } else {
        order_put(m, order);
    }

    return 0;
}

static int execute_market_ask_order(bool real, market_t *m, order_t *order)
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
        if (mpd_cmp(order->left, mpd_zero, &mpd_ctx) == 0) {
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
        mpd_mul(ask_fee, deal, order->fee, &mpd_ctx);
        mpd_mul(bid_fee, amount, pending->fee, &mpd_ctx);

        order->update_time = pending->update_time = current_timestamp();
        if (real) {
            append_order_deal_history(order->update_time, order->id, pending->id, amount, price, deal, ask_fee, bid_fee);
            push_deal_message(order->update_time, m->name, order, pending, price, amount, ask_fee, bid_fee);
        }

        mpd_sub(order->left, order->left, amount, &mpd_ctx);
        mpd_add(order->deal_stock, order->deal_stock, amount, &mpd_ctx);
        mpd_add(order->deal_money, order->deal_money, deal, &mpd_ctx);
        mpd_add(order->deal_fee, order->deal_fee, ask_fee, &mpd_ctx);

        balance_add(order->user_id, BALANCE_TYPE_AVAILABLE, m->money, deal);
        if (real) {
            append_balance_trade_add(order, m->money, m->name, deal, price, amount);
        }
        if (mpd_cmp(ask_fee, mpd_zero, &mpd_ctx) > 0) {
            balance_sub(order->user_id, BALANCE_TYPE_AVAILABLE, m->money, ask_fee);
            if (real) {
                char business[100];
                snprintf(business, sizeof(business), "%s_FEE", m->name);
                append_balance_trade_sub(order, m->money, business, ask_fee, price, amount);
            }
        }
        balance_sub(order->user_id, BALANCE_TYPE_AVAILABLE, m->stock, amount);
        if (real) {
            append_balance_trade_sub(order, m->stock, m->name, amount, price, amount);
        }

        mpd_sub(pending->left, pending->left, amount, &mpd_ctx);
        mpd_sub(pending->freeze, pending->freeze, deal, &mpd_ctx);
        mpd_add(pending->deal_stock, pending->deal_stock, amount, &mpd_ctx);
        mpd_add(pending->deal_money, pending->deal_money, deal, &mpd_ctx);
        mpd_add(pending->deal_fee, pending->deal_fee, bid_fee, &mpd_ctx);

        balance_add(pending->user_id, BALANCE_TYPE_AVAILABLE, m->stock, amount);
        if (real) {
            append_balance_trade_add(pending, m->stock, m->name, amount, price, amount);
        }
        if (mpd_cmp(bid_fee, mpd_zero, &mpd_ctx) > 0) {
            balance_sub(pending->user_id, BALANCE_TYPE_AVAILABLE, m->stock, bid_fee);
            if (real) {
                char business[100];
                snprintf(business, sizeof(business), "%s_FEE", m->name);
                append_balance_trade_sub(pending, m->stock, business, bid_fee, price, amount);
            }
        }
        balance_sub(pending->user_id, BALANCE_TYPE_FREEZE, m->money, deal);
        if (real) {
            append_balance_trade_sub(pending, m->money, m->name, amount, price, amount);
        }

        char *str_amount  = mpd_to_sci(amount, 0);
        char *str_price   = mpd_to_sci(price, 0);
        char *str_ask_fee = mpd_to_sci(ask_fee, 0);
        char *str_bid_fee = mpd_to_sci(bid_fee, 0);

        log_debug("market: %s, ask uesr: %u, order: %"PRIu64", bid user: %u, order: %"PRIu64", amount: %s, price: %s, ask fee: %s, bid fee: %s",
                m->name, order->user_id, order->id, pending->user_id, pending->id, str_amount, str_price, str_ask_fee, str_bid_fee);

        free(str_amount);
        free(str_price);
        free(str_ask_fee);
        free(str_bid_fee);

        if (mpd_cmp(pending->left, mpd_zero, &mpd_ctx) == 0) {
            order_finish(real, m, pending);
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

static int execute_market_bid_order(bool real, market_t *m, order_t *order)
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
        if (mpd_cmp(order->left, mpd_zero, &mpd_ctx) == 0) {
            break;
        }

        order_t *pending = node->value;
        mpd_copy(price, pending->price, &mpd_ctx);

        mpd_div(amount, order->left, price, &mpd_ctx);
        mpd_rescale(amount, amount, -m->stock_prec, &mpd_ctx);
        while (true) {
            mpd_mul(result, amount, price, &mpd_ctx);
            if (mpd_cmp(result, order->left, &mpd_ctx) > 0) {
                mpd_set_i32(result, -m->stock_prec, &mpd_ctx);
                mpd_pow(result, mpd_ten, result, &mpd_ctx);
                mpd_sub(amount, amount, result, &mpd_ctx);
            } else {
                break;
            }
        }

        if (mpd_cmp(amount, pending->left, &mpd_ctx) > 0) {
            mpd_copy(amount, pending->left, &mpd_ctx);
        }
        if (mpd_cmp(amount, mpd_zero, &mpd_ctx) == 0) {
            break;
        }

        mpd_mul(deal, price, amount, &mpd_ctx);
        mpd_mul(ask_fee, deal, pending->fee, &mpd_ctx);
        mpd_mul(bid_fee, amount, order->fee, &mpd_ctx);

        order->update_time = pending->update_time = current_timestamp();
        if (real) {
            append_order_deal_history(order->update_time, pending->id, order->id, amount, price, deal, ask_fee, bid_fee);
            push_deal_message(order->update_time, m->name, pending, order, price, amount, ask_fee, bid_fee);
        }

        mpd_sub(order->left, order->left, deal, &mpd_ctx);
        mpd_add(order->deal_stock, order->deal_stock, amount, &mpd_ctx);
        mpd_add(order->deal_money, order->deal_money, deal, &mpd_ctx);
        mpd_add(order->deal_fee, order->deal_fee, bid_fee, &mpd_ctx);

        balance_add(order->user_id, BALANCE_TYPE_AVAILABLE, m->stock, amount);
        if (real) {
            append_balance_trade_add(order, m->stock, m->name, amount, price, amount);
        }
        if (mpd_cmp(bid_fee, mpd_zero, &mpd_ctx) > 0) {
            balance_sub(order->user_id, BALANCE_TYPE_AVAILABLE, m->stock, bid_fee);
            if (real) {
                char business[100];
                snprintf(business, sizeof(business), "%s_FEE", m->name);
                append_balance_trade_sub(order, m->stock, business, bid_fee, price, amount);
            }
        }
        balance_sub(order->user_id, BALANCE_TYPE_AVAILABLE, m->money, deal);
        if (real) {
            append_balance_trade_sub(order, m->money, m->name, deal, price, amount);
        }

        mpd_sub(pending->left, pending->left, amount, &mpd_ctx);
        mpd_sub(pending->freeze, pending->freeze, amount, &mpd_ctx);
        mpd_add(pending->deal_stock, pending->deal_stock, amount, &mpd_ctx);
        mpd_add(pending->deal_money, pending->deal_money, deal, &mpd_ctx);
        mpd_add(pending->deal_fee, pending->deal_fee, ask_fee, &mpd_ctx);

        balance_add(pending->user_id, BALANCE_TYPE_AVAILABLE, m->money, deal);
        if (real) {
            append_balance_trade_add(pending, m->money, m->name, deal, price, amount);
        }
        if (mpd_cmp(ask_fee, mpd_zero, &mpd_ctx) > 0) {
            balance_sub(pending->user_id, BALANCE_TYPE_AVAILABLE, m->money, ask_fee);
            if (real) {
                char business[100];
                snprintf(business, sizeof(business), "%s_FEE", m->name);
                append_balance_trade_sub(pending, m->money, business, ask_fee, price, amount);
            }
        }
        balance_sub(pending->user_id, BALANCE_TYPE_FREEZE, m->stock, amount);
        if (real) {
            append_balance_trade_sub(pending, m->stock, m->name, amount, price, amount);
        }

        char *str_amount  = mpd_to_sci(amount, 0);
        char *str_price   = mpd_to_sci(price, 0);
        char *str_ask_fee = mpd_to_sci(ask_fee, 0);
        char *str_bid_fee = mpd_to_sci(bid_fee, 0);

        log_debug("market: %s, ask uesr: %u, order: %"PRIu64", bid user: %u, order: %"PRIu64", amount: %s, price: %s, ask fee: %s, bid fee: %s",
                m->name, pending->user_id, pending->id, order->user_id, order->id, str_amount, str_price, str_ask_fee, str_bid_fee);

        free(str_amount);
        free(str_price);
        free(str_ask_fee);
        free(str_bid_fee);

        if (mpd_cmp(pending->left, mpd_zero, &mpd_ctx) == 0) {
            order_finish(real, m, pending);
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

int market_put_market_order(bool real, market_t *m, uint32_t user_id, uint32_t side, mpd_t *amount, mpd_t *fee)
{
    if (side == MARKET_ORDER_SIDE_ASK) {
        mpd_t *balance = balance_get(user_id, BALANCE_TYPE_AVAILABLE, m->stock);
        if (!balance || mpd_cmp(balance, amount, &mpd_ctx) < 0) {
            return -1;
        }
    } else {
        mpd_t *balance = balance_get(user_id, BALANCE_TYPE_AVAILABLE, m->money);
        if (!balance || mpd_cmp(balance, amount, &mpd_ctx) < 0) {
            return -1;
        }
    }

    order_t *order = malloc(sizeof(order_t));
    if (order == NULL) {
        return -__LINE__;
    }

    order->id           = ++m->id_start;
    order->type         = MARKET_ORDER_TYPE_MARKET;
    order->side         = side;
    order->create_time  = current_timestamp();
    order->update_time  = order->create_time;
    order->market       = strdup(m->name);
    order->user_id      = user_id;
    order->price        = mpd_new(&mpd_ctx);
    order->amount       = mpd_new(&mpd_ctx);
    order->fee          = mpd_new(&mpd_ctx);
    order->left         = mpd_new(&mpd_ctx);
    order->freeze       = mpd_new(&mpd_ctx);
    order->deal_stock   = mpd_new(&mpd_ctx);
    order->deal_money   = mpd_new(&mpd_ctx);
    order->deal_fee     = mpd_new(&mpd_ctx);

    mpd_copy(order->amount, amount, &mpd_ctx);
    mpd_copy(order->fee, fee, &mpd_ctx);
    mpd_copy(order->left, order->amount, &mpd_ctx);
    mpd_copy(order->deal_stock, mpd_zero, &mpd_ctx);
    mpd_copy(order->deal_money, mpd_zero, &mpd_ctx);
    mpd_copy(order->deal_fee, mpd_zero, &mpd_ctx);

    int ret;
    if (side == MARKET_ORDER_SIDE_ASK) {
        ret = execute_market_ask_order(real, m, order);
    } else {
        ret = execute_market_bid_order(real, m, order);
    }
    if (ret < 0) {
        log_error("execute order: %"PRIu64" fail: %d", order->id, ret);
        order_free(order);
        return -__LINE__;
    }

    if (real) {
        int ret = append_order_history(order);
        if (ret < 0) {
            log_fatal("append_order_history fail: %d, order: %"PRIu64"", ret, order->id);
        }
    }

    order_free(order);
    return 0;
}

int market_cancel_order(bool real, market_t *m, order_t *order)
{
    return order_finish(real, m, order);
}

void market_put_order(market_t *m, order_t *order)
{
    order_put(m, order);
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

