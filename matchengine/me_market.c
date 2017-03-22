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

order_t *market_put_order(market_t *m, uint32_t user_id, \
        uint32_t order_type, mpd_t *amount, mpd_t *price, mpd_t *fee_rate)
{
    return NULL;
}

order_t *market_get_order(market_t *m, uint64_t id)
{
    return NULL;
}

list_t *market_get_order_list(market_t *m, uint32_t user_id)
{
    return NULL;
}

