/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/27, create
 */

# include "aw_config.h"
# include "aw_server.h"
# include "aw_depth.h"

static nw_timer timer;
static dict_t *dict_depth;
static rpc_clt *matchengine;
static nw_state *state_context;

# define CLEAN_INTERVAL 60

struct depth_key {
    char market[MARKET_NAME_MAX_LEN];
    char interval[INTERVAL_MAX_LEN];
    uint32_t limit;
};

struct depth_val {
    dict_t *sessions;
    json_t *last;
    time_t  last_clean;
};

struct state_data {
    struct depth_key key;
};

static uint32_t dict_ses_hash_func(const void *key)
{
    return dict_generic_hash_function(key, sizeof(void *));
}

static int dict_ses_key_compare(const void *key1, const void *key2)
{
    return key1 == key2 ? 0 : 1;
}

static uint32_t dict_depth_hash_func(const void *key)
{
    return dict_generic_hash_function(key, sizeof(struct depth_key));
}

static int dict_depth_key_compare(const void *key1, const void *key2)
{
    return memcmp(key1, key2, sizeof(struct depth_key));
}

static void *dict_depth_key_dup(const void *key)
{
    struct depth_key *obj = malloc(sizeof(struct depth_key));
    memcpy(obj, key, sizeof(struct depth_key));
    return obj;
}

static void dict_depth_key_free(void *key)
{
    free(key);
}

static void *dict_depth_val_dup(const void *val)
{
    struct depth_val *obj = malloc(sizeof(struct depth_val));
    memcpy(obj, val, sizeof(struct depth_val));
    return obj;
}

static void dict_depth_val_free(void *val)
{
    struct depth_val *obj = val;
    dict_release(obj->sessions);
    if (obj->last)
        json_decref(obj->last);
    free(obj);
}

static void on_backend_connect(nw_ses *ses, bool result)
{
    rpc_clt *clt = ses->privdata;
    if (result) {
        log_info("connect %s:%s success", clt->name, nw_sock_human_addr(&ses->peer_addr));
    } else {
        log_info("connect %s:%s fail", clt->name, nw_sock_human_addr(&ses->peer_addr));
    }
}

static json_t *get_list_diff(json_t *list1, json_t *list2, uint32_t limit, int side)
{
    if (list1 == NULL || list2 == NULL)
        return NULL;

    json_t *diff = json_array();
    mpd_t *price1  = NULL;
    mpd_t *amount1 = NULL;
    mpd_t *price2  = NULL;
    mpd_t *amount2 = NULL;

    size_t list1_size = json_array_size(list1);
    size_t list2_size = json_array_size(list2);
    size_t list1_pos = 0;
    size_t list2_pos = 0;

    while (list1_pos < list1_size && list2_pos < list2_size) {
        json_t *unit1 = json_array_get(list1, list1_pos);
        const char *unit1_price  = json_string_value(json_array_get(unit1, 0));
        const char *unit1_amount = json_string_value(json_array_get(unit1, 1));
        if (unit1_price == NULL || unit1_amount == NULL)
            goto error;
        price1 = decimal(unit1_price, 0);
        amount1 = decimal(unit1_amount, 0);
        if (price1 == NULL || amount1 == NULL)
            goto error;

        json_t *unit2 = json_array_get(list2, list2_pos);
        const char *unit2_price  = json_string_value(json_array_get(unit2, 0));
        const char *unit2_amount = json_string_value(json_array_get(unit2, 1));
        if (unit2_price == NULL || unit2_amount == NULL)
            goto error;
        price2 = decimal(unit2_price, 0);
        amount2 = decimal(unit2_amount, 0);
        if (price2 == NULL || amount2 == NULL)
            goto error;

        int cmp = mpd_cmp(price1, price2, &mpd_ctx) * side;
        if (cmp == 0) {
            list1_pos += 1;
            list2_pos += 1;
            if (mpd_cmp(amount1, amount2, &mpd_ctx) != 0) {
                json_array_append(diff, unit2);
            }
        } else if (cmp > 0) {
            list2_pos += 1;
            json_array_append(diff, unit2);
        } else {
            list1_pos += 1;
            json_t *unit = json_array();
            json_array_append_new_mpd(unit, price1);
            json_array_append_new(unit, json_string("0"));
            json_array_append_new(diff, unit);
        }

        mpd_del(price1);
        price1  = NULL;
        mpd_del(amount1);
        amount1 = NULL;
        mpd_del(price2);
        price2  = NULL;
        mpd_del(amount2);
        amount2 = NULL;
    }

    while (list2_size < limit && list1_pos < list1_size) {
        json_t *unit = json_array_get(list1, list1_pos);
        const char *price = json_string_value(json_array_get(unit, 0));
        const char *amount = json_string_value(json_array_get(unit, 1));
        if (price == NULL || amount == NULL)
            goto error;
        json_t *new = json_array();
        json_array_append_new(new, json_string(price));
        json_array_append_new(new, json_string("0"));
        json_array_append_new(diff, new);
        list1_pos += 1;
    }

    for (;list2_pos < list2_size; ++list2_pos) {
        json_array_append(diff, json_array_get(list2, list2_pos));
    }

    if (json_array_size(diff) == 0) {
        json_decref(diff);
        return NULL;
    }

    return diff;

error:
    json_decref(diff);
    if (price1)
        mpd_del(price1);
    if (amount1)
        mpd_del(amount1);
    if (price2)
        mpd_del(price2);
    if (amount2)
        mpd_del(amount2);

    return NULL;
}

static json_t *get_depth_diff(json_t *first, json_t *second, uint32_t limit)
{
    json_t *asks = get_list_diff(json_object_get(first, "asks"), json_object_get(second, "asks"), limit,  1);
    json_t *bids = get_list_diff(json_object_get(first, "bids"), json_object_get(second, "bids"), limit, -1);
    if (asks == NULL && bids == NULL)
        return NULL;
    json_t *diff = json_object();
    if (asks)
        json_object_set_new(diff, "asks", asks);
    if (bids)
        json_object_set_new(diff, "bids", bids);
    return diff;
}

static int broadcast_update(const char *market, dict_t *sessions, bool clean, json_t *result)
{
    json_t *params = json_array();
    json_array_append_new(params, json_boolean(clean));
    json_array_append(params, result);
    json_array_append_new(params, json_string(market));

    dict_iterator *iter = dict_get_iterator(sessions);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        send_notify(entry->key, "depth.update", params);
    }
    dict_release_iterator(iter);
    json_decref(params);

    return 0;
}

static int on_market_depth_reply(struct state_data *state, json_t *result)
{
    dict_entry *entry = dict_find(dict_depth, &state->key);
    if (entry == NULL)
        return -__LINE__;
    struct depth_key *key = entry->key;
    struct depth_val *val = entry->val;
    if (val->last == NULL) {
        val->last = result;
        val->last_clean = time(NULL);
        json_incref(result);
        return broadcast_update(state->key.market, val->sessions, true, result);
    }

    json_t *diff = get_depth_diff(val->last, result, key->limit);
    if (diff == NULL) {
        return 0;
    }

    json_decref(val->last);
    val->last = result;
    json_incref(result);

    time_t now = time(NULL);
    if (now - val->last_clean >= CLEAN_INTERVAL) {
        val->last_clean = now;
        broadcast_update(state->key.market, val->sessions, true, result);
    } else {
        broadcast_update(state->key.market, val->sessions, false, diff);
    }
    json_decref(diff);

    return 0;
}

static void on_backend_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
    sds reply_str = sdsnewlen(pkg->body, pkg->body_size);
    log_trace("recv pkg from: %s, cmd: %u, sequence: %u, reply: %s",
            nw_sock_human_addr(&ses->peer_addr), pkg->command, pkg->sequence, reply_str);
    nw_state_entry *entry = nw_state_get(state_context, pkg->sequence);
    if (entry == NULL) {
        sdsfree(reply_str);
        return;
    }
    struct state_data *state = entry->data;

    json_t *reply = json_loadb(pkg->body, pkg->body_size, 0, NULL);
    if (reply == NULL) {
        sds hex = hexdump(pkg->body, pkg->body_size);
        log_fatal("invalid reply from: %s, cmd: %u, reply: \n%s", nw_sock_human_addr(&ses->peer_addr), pkg->command, hex);
        sdsfree(hex);
        sdsfree(reply_str);
        nw_state_del(state_context, pkg->sequence);
        return;
    }

    json_t *error = json_object_get(reply, "error");
    if (error && !json_is_null(error)) {
        dict_delete(dict_depth, &state->key);
    }
    json_t *result = json_object_get(reply, "result");
    if (error == NULL || !json_is_null(error) || result == NULL) {
        log_error("error reply from: %s, cmd: %u, reply: %s", nw_sock_human_addr(&ses->peer_addr), pkg->command, reply_str);
        sdsfree(reply_str);
        json_decref(reply);
        nw_state_del(state_context, pkg->sequence);
        return;
    }

    int ret;
    switch (pkg->command) {
    case CMD_ORDER_BOOK_DEPTH:
        ret = on_market_depth_reply(state, result);
        if (ret < 0) {
            log_error("on_market_depth_reply: %d, reply: %s", ret, reply_str);
        }
        break;
    default:
        log_error("recv unknown command: %u from: %s", pkg->command, nw_sock_human_addr(&ses->peer_addr));
        break;
    }
    
    sdsfree(reply_str);
    json_decref(reply);
    nw_state_del(state_context, pkg->sequence);
}

static void on_timeout(nw_state_entry *entry)
{
    log_fatal("query depth timeout, state id: %u", entry->id);
}

static void on_timer(nw_timer *timer, void *privdata)
{
    dict_iterator *iter = dict_get_iterator(dict_depth);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        const struct depth_val *obj = entry->val;
        if (dict_size(obj->sessions) == 0) {
            dict_delete(dict_depth, entry->key);
            continue;
        }

        struct depth_key *key = entry->key;
        json_t *params = json_array();
        json_array_append_new(params, json_string(key->market));
        json_array_append_new(params, json_integer(key->limit));
        json_array_append_new(params, json_string(key->interval));

        nw_state_entry *state_entry = nw_state_add(state_context, settings.backend_timeout, 0);
        struct state_data *state = state_entry->data;
        memcpy(&state->key, key, sizeof(struct depth_key));

        rpc_pkg pkg;
        memset(&pkg, 0, sizeof(pkg));
        pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
        pkg.command   = CMD_ORDER_BOOK_DEPTH;
        pkg.sequence  = state_entry->id;
        pkg.body      = json_dumps(params, 0);
        pkg.body_size = strlen(pkg.body);

        rpc_clt_send(matchengine, &pkg);
        log_trace("send request to %s, cmd: %u, sequence: %u, params: %s",
                nw_sock_human_addr(rpc_clt_peer_addr(matchengine)), pkg.command, pkg.sequence, (char *)pkg.body);
        free(pkg.body);
        json_decref(params);
    }
    dict_release_iterator(iter);
}

int init_depth(void)
{
    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function = dict_depth_hash_func;
    dt.key_compare = dict_depth_key_compare;
    dt.key_dup = dict_depth_key_dup;
    dt.key_destructor = dict_depth_key_free;
    dt.val_dup = dict_depth_val_dup;
    dt.val_destructor = dict_depth_val_free;

    dict_depth = dict_create(&dt, 64);
    if (dict_depth == NULL)
        return -__LINE__;

    rpc_clt_type ct;
    memset(&ct, 0, sizeof(ct));
    ct.on_connect = on_backend_connect;
    ct.on_recv_pkg = on_backend_recv_pkg;

    matchengine = rpc_clt_create(&settings.matchengine, &ct);
    if (matchengine == NULL)
        return -__LINE__;
    if (rpc_clt_start(matchengine) < 0)
        return -__LINE__;

    nw_state_type st;
    memset(&st, 0, sizeof(st));
    st.on_timeout = on_timeout;

    state_context = nw_state_create(&st, sizeof(struct state_data));
    if (state_context == NULL)
        return -__LINE__;

    nw_timer_set(&timer, settings.depth_interval, true, on_timer, NULL);
    nw_timer_start(&timer);

    return 0;
}

static bool is_good_limit(int limit)
{
    for (int i = 0; i < settings.depth_limit.count; ++i) {
        if (settings.depth_limit.limit[i] == limit) {
            return true;
        }
    }

    return false;
}

static bool is_good_interval(const char *interval)
{
    mpd_t *merge = decimal(interval, 0);
    if (merge == NULL)
        return false;

    for (int i = 0; i < settings.depth_merge.count; ++i) {
        if (mpd_cmp(settings.depth_merge.limit[i], merge, &mpd_ctx) == 0) {
            mpd_del(merge);
            return true;
        }
    }

    mpd_del(merge);
    return false;
}

int depth_subscribe(nw_ses *ses, const char *market, uint32_t limit, const char *interval)
{
    if (!is_good_limit(limit))
        return -1;
    if (!is_good_interval(interval))
        return -1;

    struct depth_key key;
    memset(&key, 0, sizeof(key));
    strncpy(key.market, market, MARKET_NAME_MAX_LEN - 1);
    strncpy(key.interval, interval, INTERVAL_MAX_LEN - 1);
    key.limit = limit;

    dict_entry *entry = dict_find(dict_depth, &key);
    if (entry == NULL) {
        struct depth_val val;
        memset(&val, 0, sizeof(val));

        dict_types dt;
        memset(&dt, 0, sizeof(dt));
        dt.hash_function = dict_ses_hash_func;
        dt.key_compare = dict_ses_key_compare;
        val.sessions = dict_create(&dt, 1024);
        if (val.sessions == NULL)
            return -__LINE__;

        entry = dict_add(dict_depth, &key, &val);
        if (entry == NULL)
            return -__LINE__;
    }

    struct depth_val *obj = entry->val;
    dict_add(obj->sessions, ses, NULL);

    return 0;
}

int depth_unsubscribe(nw_ses *ses)
{
    dict_iterator *iter = dict_get_iterator(dict_depth);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct depth_val *obj = entry->val;
        dict_delete(obj->sessions, ses);
    }
    dict_release_iterator(iter);

    return 0;
}

int depth_send_clean(nw_ses *ses, const char *market, uint32_t limit, const char *interval)
{
    struct depth_key key;
    memset(&key, 0, sizeof(key));
    strncpy(key.market, market, MARKET_NAME_MAX_LEN - 1);
    strncpy(key.interval, interval, INTERVAL_MAX_LEN - 1);
    key.limit = limit;

    dict_entry *entry = dict_find(dict_depth, &key);
    if (entry == NULL)
        return 0;

    struct depth_val *obj = entry->val;
    if (obj->last) {
        json_t *params = json_array();
        json_array_append_new(params, json_boolean(true));
        json_array_append(params, obj->last);
        json_array_append(params, json_string(market));
        send_notify(ses, "depth.update", params);
        json_decref(params);
    }

    return 0;
}

