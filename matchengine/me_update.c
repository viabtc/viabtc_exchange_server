/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/18, create
 */

# include "me_config.h"
# include "me_update.h"
# include "me_balance.h"
# include "me_history.h"
# include "me_message.h"

static dict_t *dict_update;
static nw_timer timer;

struct update_key {
    uint32_t    user_id;
    char        asset[ASSET_NAME_MAX_LEN + 1];
    char        business[BUSINESS_NAME_MAX_LEN + 1];
    uint64_t    business_id;
};

struct update_val {
    double      create_time;
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

static void *update_dict_val_dup(const void *val)
{
    struct update_val*obj = malloc(sizeof(struct update_val));
    memcpy(obj, val, sizeof(struct update_val));
    return obj;
}

static void update_dict_val_free(void *val)
{
    free(val);
}

static void on_timer(nw_timer *t, void *privdata)
{
    double now = current_timestamp();
    dict_iterator *iter = dict_get_iterator(dict_update);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct update_val *val = entry->val;
        if (val->create_time < (now - 86400)) {
            dict_delete(dict_update, entry->key);
        }
    }
    dict_release_iterator(iter);
}

int init_update(void)
{
    dict_types type;
    memset(&type, 0, sizeof(type));
    type.hash_function  = update_dict_hash_function;
    type.key_compare    = update_dict_key_compare;
    type.key_dup        = update_dict_key_dup;
    type.key_destructor = update_dict_key_free;
    type.val_dup        = update_dict_val_dup;
    type.val_destructor = update_dict_val_free;

    dict_update = dict_create(&type, 64);
    if (dict_update == NULL)
        return -__LINE__;

    nw_timer_set(&timer, 60, true, on_timer, NULL);
    nw_timer_start(&timer);

    return 0;
}

int update_user_balance(bool real, uint32_t user_id, const char *asset, const char *business, uint64_t business_id, mpd_t *change, json_t *detail)
{
    struct update_key key;
    key.user_id = user_id;
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
        result = balance_add(user_id, BALANCE_TYPE_AVAILABLE, asset, abs_change);
    } else {
        result = balance_sub(user_id, BALANCE_TYPE_AVAILABLE, asset, abs_change);
    }
    mpd_del(abs_change);
    if (result == NULL)
        return -2;

    struct update_val val = { .create_time = current_timestamp() };
    dict_add(dict_update, &key, &val);

    if (real) {
        double now = current_timestamp();
        json_object_set_new(detail, "id", json_integer(business_id));
        char *detail_str = json_dumps(detail, 0);
        append_user_balance_history(now, user_id, asset, business, change, detail_str);
        free(detail_str);
        push_balance_message(now, user_id, asset, business, change);
    }

    return 0;
}

