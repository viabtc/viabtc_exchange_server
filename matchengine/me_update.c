/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/18, create
 */

# include "me_config.h"
# include "me_update.h"
# include "me_balance.h"

static dict_t *dict_update;

struct update_key {
    uint32_t    user_id;
    uint32_t    type;
    char        asset[ASSET_NAME_MAX_LEN + 1];
    char        business[BUSINESS_NAME_MAX_LEN + 1];
    uint64_t    business_id;
};

static uint32_t update_dict_hash_function(const void *key)
{
    return dict_generic_hash_function(key, sizeof(struct update_key));
}

static int update_dict_key_compare(const void *key1, const void *key2)
{
    return memcmp(key1, key2, sizeof(struct update_key));
}

static void *update_dict_key_dup(const void *key)
{
    struct update_key *obj = malloc(sizeof(struct update_key));
    memcpy(obj, key, sizeof(struct update_key));
    return obj;
}

static void update_dict_key_free(void *key)
{
    free(key);
}

int init_update(void)
{
    dict_types type;
    memset(&type, 0, sizeof(type));
    type.hash_function  = update_dict_hash_function;
    type.key_compare    = update_dict_key_compare;
    type.key_dup        = update_dict_key_dup;
    type.key_destructor = update_dict_key_free;

    dict_update = dict_create(&type, 64);
    if (dict_update == NULL)
        return -__LINE__;

    return 0;
}

int update_user_balance(uint32_t user_id, uint32_t type, const char *asset, \
        mpd_t *change, const char *business, uint64_t business_id)
{
    struct update_key key;
    key.user_id = user_id;
    key.type = type;
    strncpy(key.asset, asset, sizeof(key.asset));
    strncpy(key.business, business, sizeof(key.business));
    key.business_id = business_id;

    dict_entry *entry = dict_find(dict_update, &key);
    if (entry) {
        return -1;
    }

    mpd_t *result;
    mpd_t *abs_change = mpd_new(&mpd_ctx);
    mpd_abs(abs_change, change, &mpd_ctx);
    if (mpd_cmp(change, mpd_zero, &mpd_ctx) >= 0) {
        result = balance_add(user_id, type, asset, abs_change);
    } else {
        result = balance_sub(user_id, type, asset, abs_change);
    }
    mpd_del(abs_change);
    if (result == NULL)
        return -2;

    dict_add(dict_update, &key, NULL);
    return 0;
}

