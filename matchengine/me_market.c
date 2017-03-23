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
    if (order1->type == MARKET_ORDER_TYPE_ASK) {
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

    return 0;
}

void order_free(order_t *order)
{
}

void order_finish(market_t *m, order_t *order)
{
}

market_t *market_create(struct market *conf)
{
    if (asset_exist(conf->stock) < 0 || asset_exist(conf->money) < 0)
        return NULL;

    market_t *m = malloc(sizeof(market_t));
    m->name  = strdup(conf->name);
    m->stock = strdup(conf->stock);
    m->money = strdup(conf->money);

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

int execute_ask_order(market_t *m, order_t *order)
{
    mpd_t *price = mpd_new(&mpd_ctx);
    mpd_t *amount = mpd_new(&mpd_ctx);
    mpd_t *deal = mpd_new(&mpd_ctx);
    mpd_t *ask_fee = mpd_new(&mpd_ctx);
    mpd_t *bid_fee = mpd_new(&mpd_ctx);
    mpd_t *result = mpd_new(&mpd_ctx);

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

        mpd_mul(result, price, amount, &mpd_ctx);
        mpd_rescale(deal, result, -asset_exist(m->money), &mpd_ctx);

        mpd_mul(result, deal, order->fee_rate, &mpd_ctx);
        mpd_rescale(ask_fee, result, -asset_exist(m->money), &mpd_ctx);

        mpd_mul(result, amount, pending->fee_rate, &mpd_ctx);
        mpd_rescale(bid_fee, result, -asset_exist(m->stock), &mpd_ctx);

        mpd_sub(result, order->left, amount, &mpd_ctx);
        mpd_copy(order->left, result, &mpd_ctx);
        mpd_add(result, order->deal_stock, amount, &mpd_ctx);
        mpd_copy(order->deal_stock, result, &mpd_ctx);
        mpd_add(result, order->deal_money, deal, &mpd_ctx);
        mpd_copy(order->deal_money, result, &mpd_ctx);
        mpd_add(result, order->deal_fee, ask_fee, &mpd_ctx);
        mpd_copy(order->deal_fee, result, &mpd_ctx);

        mpd_sub(result, deal, ask_fee, &mpd_ctx);
        balance_add(order->user_id, BALANCE_TYPE_AVAILABLE, m->money, result);
        balance_sub(order->user_id, BALANCE_TYPE_FREEZE, m->stock, amount);

        mpd_sub(result, pending->left, amount, &mpd_ctx);
        mpd_copy(pending->left, result, &mpd_ctx);
        mpd_add(result, pending->deal_stock, amount, &mpd_ctx);
        mpd_copy(pending->deal_stock, result, &mpd_ctx);
        mpd_add(result, pending->deal_money, deal, &mpd_ctx);
        mpd_copy(pending->deal_money, result, &mpd_ctx);
        mpd_add(result, pending->deal_fee, bid_fee, &mpd_ctx);
        mpd_copy(pending->deal_fee, result, &mpd_ctx);

        mpd_sub(result, amount, bid_fee, &mpd_ctx);
        balance_add(pending->user_id, BALANCE_TYPE_AVAILABLE, m->stock, result);
        balance_sub(pending->user_id, BALANCE_TYPE_FREEZE, m->money, deal);

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

int execute_bid_order(market_t *m, order_t *order)
{
    mpd_t *price = mpd_new(&mpd_ctx);
    mpd_t *amount = mpd_new(&mpd_ctx);
    mpd_t *deal = mpd_new(&mpd_ctx);
    mpd_t *ask_fee = mpd_new(&mpd_ctx);
    mpd_t *bid_fee = mpd_new(&mpd_ctx);
    mpd_t *result = mpd_new(&mpd_ctx);

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

        mpd_t *price = mpd_new(&mpd_ctx);
        mpd_copy(price, pending->price, &mpd_ctx);
        mpd_t *amount = mpd_new(&mpd_ctx);
        if (mpd_cmp(order->left, pending->left, &mpd_ctx) < 0) {
            mpd_copy(amount, order->left, &mpd_ctx);
        } else {
            mpd_copy(amount, pending->left, &mpd_ctx);
        }

        mpd_t *deal = mpd_new(&mpd_ctx);
        mpd_t *result = mpd_new(&mpd_ctx);
        mpd_mul(result, price, amount, &mpd_ctx);
        mpd_rescale(deal, result, -asset_exist(m->money), &mpd_ctx);

        mpd_t *ask_fee = mpd_new(&mpd_ctx);
        mpd_mul(result, deal, pending->fee_rate, &mpd_ctx);
        mpd_rescale(ask_fee, result, -asset_exist(m->money), &mpd_ctx);

        mpd_t *bid_fee = mpd_new(&mpd_ctx);
        mpd_mul(result, amount, order->fee_rate, &mpd_ctx);
        mpd_rescale(bid_fee, result, -asset_exist(m->stock), &mpd_ctx);

        mpd_sub(result, order->left, amount, &mpd_ctx);
        mpd_copy(order->left, result, &mpd_ctx);
        mpd_add(result, order->deal_stock, amount, &mpd_ctx);
        mpd_copy(order->deal_stock, result, &mpd_ctx);
        mpd_add(result, order->deal_money, deal, &mpd_ctx);
        mpd_copy(order->deal_money, result, &mpd_ctx);
        mpd_add(result, order->deal_fee, bid_fee, &mpd_ctx);
        mpd_copy(order->deal_fee, result, &mpd_ctx);

        mpd_sub(result, amount, bid_fee, &mpd_ctx);
        balance_add(order->user_id, BALANCE_TYPE_AVAILABLE, m->stock, result);
        balance_sub(order->user_id, BALANCE_TYPE_FREEZE, m->money, deal);

        mpd_sub(result, pending->left, amount, &mpd_ctx);
        mpd_copy(pending->left, result, &mpd_ctx);
        mpd_add(result, pending->deal_stock, amount, &mpd_ctx);
        mpd_copy(pending->deal_stock, result, &mpd_ctx);
        mpd_add(result, pending->deal_money, deal, &mpd_ctx);
        mpd_copy(pending->deal_money, result, &mpd_ctx);
        mpd_add(result, pending->deal_fee, ask_fee, &mpd_ctx);
        mpd_copy(pending->deal_fee, result, &mpd_ctx);

        mpd_sub(result, deal, ask_fee, &mpd_ctx);
        balance_add(pending->user_id, BALANCE_TYPE_AVAILABLE, m->money, result);
        balance_sub(pending->user_id, BALANCE_TYPE_FREEZE, m->stock, amount);

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

order_t *market_put_limit_order(market_t *m, uint32_t user_id, \
        uint32_t order_type, mpd_t *amount, mpd_t *price, mpd_t *fee_rate)
{
    if (mpd_cmp(amount, mpd_zero, &mpd_ctx) <= 0)
        return NULL;
    if (mpd_cmp(price, mpd_zero, &mpd_ctx) <= 0)
        return NULL;
    if (mpd_cmp(fee_rate, mpd_zero, &mpd_ctx) < 0)
        return NULL;
    if (mpd_cmp(fee_rate, mpd_one, &mpd_ctx) >= 0)
        return NULL;

    order_t *order = malloc(sizeof(order_t));
    if (order == NULL)
        return NULL;

    order->id = ++m->id_start;
    order->create_time  = current_timestamp();
    order->update_time  = current_timestamp();
    order->market_name  = strdup(m->name);
    order->user_id      = user_id;
    order->type         = order_type;
    order->price        = mpd_new(&mpd_ctx);
    order->amount       = mpd_new(&mpd_ctx);
    order->fee_rate     = mpd_new(&mpd_ctx);
    order->left         = mpd_new(&mpd_ctx);
    order->deal_stock   = mpd_new(&mpd_ctx);
    order->deal_money   = mpd_new(&mpd_ctx);
    order->deal_fee     = mpd_new(&mpd_ctx);

    mpd_copy(order->price, price, &mpd_ctx);
    mpd_copy(order->amount, amount, &mpd_ctx);
    mpd_copy(order->fee_rate, fee_rate, &mpd_ctx);
    mpd_copy(order->left, amount, &mpd_ctx);
    mpd_copy(order->deal_stock, mpd_zero, &mpd_ctx);
    mpd_copy(order->deal_money, mpd_zero, &mpd_ctx);
    mpd_copy(order->deal_fee, mpd_zero, &mpd_ctx);

    int ret;
    if (order_type == MARKET_ORDER_TYPE_ASK) {
        ret = execute_ask_order(m, order);
    } else {
        ret = execute_bid_order(m, order);
    }
    if (ret < 0) {
        order_free(order);
        return NULL;
    }

    if (mpd_cmp(order->left, mpd_zero, &mpd_ctx) == 0) {
        return order;
    }

    return order;
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

