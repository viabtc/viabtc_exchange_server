/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/15, create
 */

# include "me_config.h"
# include "me_balance.h"

static dict_t *dict_asset;
static dict_t *dict_balance;

struct asset_type {
    int         prec;
};

struct balance_key {
    uint32_t    user_id;
    uint32_t    type;
    char        asset[ASSET_NAME_MAX_LEN + 1];
};

static uint32_t asset_dict_hash_function(const void *key)
{
    return dict_generic_hash_function(key, strlen(key));
}

static void *asset_dict_key_dup(const void *key)
{
    return strdup(key);
}

static void *asset_dict_val_dup(const void *val)
{
    struct asset_type *obj = malloc(sizeof(struct asset_type));
    memcpy(obj, val, sizeof(struct asset_type));
    return obj;
}

static int asset_dict_key_compare(const void *key1, const void *key2)
{
    return strcmp(key1, key2);
}

static void asset_dict_key_free(void *key)
{
    free(key);
}

static void asset_dict_val_free(void *val)
{
    free(val);
}

static uint32_t balance_dict_hash_function(const void *key)
{
    return dict_generic_hash_function(key, sizeof(struct balance_key));
}

static void *balance_dict_key_dup(const void *key)
{
    struct balance_key *obj = malloc(sizeof(struct balance_key));
    memcpy(obj, key, sizeof(struct balance_key));
    return obj;
}

static void *balance_dict_val_dup(const void *val)
{
    return mpd_qncopy(val);
}

static int balance_dict_key_compare(const void *key1, const void *key2)
{
    return memcmp(key1, key2, sizeof(struct balance_key));
}

static void balance_dict_key_free(void *key)
{
    free(key);
}

static void balance_dict_val_free(void *val)
{
    mpd_del(val);
}

static int init_dict(void)
{
    dict_types type;
    memset(&type, 0, sizeof(type));
    type.hash_function  = asset_dict_hash_function;
    type.key_compare    = asset_dict_key_compare;
    type.key_dup        = asset_dict_key_dup;
    type.key_destructor = asset_dict_key_free;
    type.val_dup        = asset_dict_val_dup;
    type.val_destructor = asset_dict_val_free;

    dict_asset = dict_create(&type, 64);
    if (dict_asset == NULL)
        return -__LINE__;

    memset(&type, 0, sizeof(type));
    type.hash_function  = balance_dict_hash_function;
    type.key_compare    = balance_dict_key_compare;
    type.key_dup        = balance_dict_key_dup;
    type.key_destructor = balance_dict_key_free;
    type.val_dup        = balance_dict_val_dup;
    type.val_destructor = balance_dict_val_free;

    dict_balance = dict_create(&type, 64);
    if (dict_balance == NULL)
        return -__LINE__;

    return 0;
}

int init_balance()
{
    ERR_RET(init_dict());
    for (size_t i = 0; i < settings.asset_num; ++i) {
        struct asset_type type = { .prec = settings.assets[i].prec };
        if (dict_add(dict_asset, settings.assets[i].name, &type) < 0)
            return -__LINE__;
    }

    return 0;
}

bool asset_exist(const char *asset)
{
    if (dict_find(dict_asset, asset))
        return true;
    return false;
}

static struct asset_type *get_asset_type(const char *asset)
{
    dict_entry *entry = dict_find(dict_asset, asset);
    if (entry == NULL)
        return NULL;
    return entry->val;
}
    

mpd_t *balance_get(uint32_t user_id, uint32_t type, const char *asset)
{
    struct balance_key key;
    key.user_id = user_id;
    key.type = type;
    strncpy(key.asset, asset, sizeof(key.asset));

    dict_entry *entry = dict_find(dict_balance, &key);
    if (entry) {
        return entry->val;
    }

    return NULL;
}

mpd_t *balance_set(uint32_t user_id, uint32_t type, const char *asset, mpd_t *amount)
{
    struct asset_type *at = get_asset_type(asset);
    if (at == NULL)
        return NULL;

    struct balance_key key;
    key.user_id = user_id;
    key.type = type;
    strncpy(key.asset, asset, sizeof(key.asset));

    mpd_t *result;
    dict_entry *entry = dict_find(dict_balance, &key);
    if (entry) {
        result = entry->val;
        mpd_rescale(result, amount, -at->prec, &mpd_ctx);
        return result;
    }

    if (dict_add(dict_balance, &key, amount) < 0)
        return NULL;
    dict_entry *entry = dict_find(dict_balance, &key);
    if (entry == NULL)
        return NULL;
    result = entry->val;
    mpd_rescale(result, amount, -at->prec, &mpd_ctx);
    return result;
}

mpd_t *balance_add(uint32_t user_id, uint32_t type, const char *asset, mpd_t *amount)
{
    struct asset_type *at = get_asset_type(asset);
    if (at == NULL)
        return NULL;

    if (mpd_cmp(amount, mpd_zero, &mpd_ctx) < 0)
        return NULL;

    struct balance_key key;
    key.user_id = user_id;
    key.type = type;
    strncpy(key.asset, asset, sizeof(key.asset));

    mpd_t *result;
    dict_entry *entry = dict_find(dict_balance, &key);
    if (entry) {
        result = entry->val;
        mpd_t *tmp = mpd_new(&mpd_ctx);
        mpd_add(tmp, result, amount, &mpd_ctx);
        mpd_rescale(result, tmp, -at->prec, &mpd_ctx);
        mpd_del(tmp);
        return result;
    }

    return balance_set(user_id, type, asset, amount);
}

mpd_t *balance_sub(uint32_t user_id, uint32_t type, const char *asset, mpd_t *amount)
{
    struct asset_type *at = get_asset_type(asset);
    if (at == NULL)
        return NULL;

    mpd_t *result = balance_get(user_id, type, asset);
    if (result == NULL)
        return NULL;
    if (mpd_cmp(result, amount, &mpd_ctx) < 0)
        return NULL;

    mpd_t *tmp = mpd_new(&mpd_ctx);
    mpd_sub(tmp, result, amount, &mpd_ctx);
    mpd_rescale(result, tmp, -at->prec, &mpd_ctx);
    mpd_del(tmp);

    return result;
}

mpd_t *balance_freeze(uint32_t user_id, const char *asset, mpd_t *amount)
{
    struct asset_type *at = get_asset_type(asset);
    if (at == NULL)
        return NULL;

    if (mpd_cmp(amount, mpd_zero, &mpd_ctx) < 0)
        return NULL;
    mpd_t *available = balance_get(user_id, BALANCE_TYPE_AVAILABLE, asset);
    if (available == NULL)
        return NULL;
    if (mpd_cmp(available, amount, &mpd_ctx) < 0)
        return NULL;

    if (balance_add(user_id, BALANCE_TYPE_FREEZE, asset, amount) == 0)
        return NULL;
    mpd_t *tmp = mpd_new(&mpd_ctx);
    mpd_sub(tmp, available, amount, &mpd_ctx);
    mpd_rescale(available, tmp, -at->prec, &mpd_ctx);
    mpd_del(tmp);

    return available;
}

mpd_t *balance_unfreeze(uint32_t user_id, const char *asset, mpd_t *amount)
{
    struct asset_type *at = get_asset_type(asset);
    if (at == NULL)
        return NULL;

    if (mpd_cmp(amount, mpd_zero, &mpd_ctx) < 0)
        return NULL;
    mpd_t *freeze = balance_get(user_id, BALANCE_TYPE_FREEZE, asset);
    if (freeze == NULL)
        return NULL;
    if (mpd_cmp(freeze, amount, &mpd_ctx) < 0)
        return NULL;

    if (balance_add(user_id, BALANCE_TYPE_AVAILABLE, asset, amount) == 0)
        return NULL;
    mpd_t *tmp = mpd_new(&mpd_ctx);
    mpd_sub(tmp, freeze, amount, &mpd_ctx);
    mpd_rescale(freeze, tmp, -at->prec, &mpd_ctx);
    mpd_del(tmp);

    return freeze;
}

