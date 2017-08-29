/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/28, create
 */

# include "aw_config.h"
# include "aw_price.h"
# include "aw_server.h"

static nw_timer timer;
static dict_t *dict_market;
static rpc_clt *marketprice;
static nw_state *state_context;

struct state_data {
    char market[MARKET_NAME_MAX_LEN];
};

struct market_val {
    dict_t *sessions;
    mpd_t  *last;
};

static uint32_t dict_ses_hash_func(const void *key)
{
    return dict_generic_hash_function(key, sizeof(void *));
}

static int dict_ses_key_compare(const void *key1, const void *key2)
{
    return key1 == key2 ? 0 : 1;
}

static uint32_t dict_market_hash_func(const void *key)
{
    return dict_generic_hash_function(key, strlen(key));
}

static int dict_market_key_compare(const void *key1, const void *key2)
{
    return strcmp(key1, key2);
}

static void *dict_market_key_dup(const void *key)
{
    return strdup(key);
}

static void dict_market_key_free(void *key)
{
    free(key);
}

static void *dict_market_val_dup(const void *key)
{
    struct market_val *obj = malloc(sizeof(struct market_val));
    memcpy(obj, key, sizeof(struct market_val));
    return obj;
}

static void dict_market_val_free(void *val)
{
    struct market_val *obj = val;
    dict_release(obj->sessions);
    mpd_del(obj->last);
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

static int on_market_last_reply(struct state_data *state, json_t *result)
{
    dict_entry *entry = dict_find(dict_market, state->market);
    if (entry == NULL)
        return -__LINE__;
    struct market_val *obj = entry->val;

    if (!json_is_string(result))
        return -__LINE__;
    mpd_t *last = decimal(json_string_value(result), 0);
    if (last == NULL)
        return -__LINE__;

    if (mpd_cmp(last, obj->last, &mpd_ctx) != 0) {
        mpd_copy(obj->last, last, &mpd_ctx);

        json_t *params = json_array();
        json_array_append_new(params, json_string(state->market));
        json_array_append(params, result);

        dict_iterator *iter = dict_get_iterator(obj->sessions);
        dict_entry *entry;
        while ((entry = dict_next(iter)) != NULL) {
            send_notify(entry->key, "price.update", params);
        }
        dict_release_iterator(iter);
        json_decref(params);
    }

    mpd_del(last);
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
        dict_delete(dict_market, state->market);
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
    case CMD_MARKET_LAST:
        ret = on_market_last_reply(state, result);
        if (ret < 0) {
            log_error("on_market_last_reply: %d, reply: %s", ret, reply_str);
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
    log_fatal("query last price timeout, state id: %u", entry->id);
}

static void on_timer(nw_timer *timer, void *privdata)
{
    dict_iterator *iter = dict_get_iterator(dict_market);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        const struct market_val *obj = entry->val;
        if (dict_size(obj->sessions) == 0) {
            dict_delete(dict_market, entry->key);
            continue;
        }

        const char *market = entry->key;
        json_t *params = json_array();
        json_array_append_new(params, json_string(market));

        nw_state_entry *state_entry = nw_state_add(state_context, settings.backend_timeout, 0);
        struct state_data *state = state_entry->data;
        strncpy(state->market, market, MARKET_NAME_MAX_LEN - 1);

        rpc_pkg pkg;
        memset(&pkg, 0, sizeof(pkg));
        pkg.pkg_type  = RPC_PKG_TYPE_REQUEST;
        pkg.command   = CMD_MARKET_LAST;
        pkg.sequence  = state_entry->id;
        pkg.body      = json_dumps(params, 0);
        pkg.body_size = strlen(pkg.body);

        rpc_clt_send(marketprice, &pkg);
        log_trace("send request to %s, cmd: %u, sequence: %u, params: %s",
                nw_sock_human_addr(rpc_clt_peer_addr(marketprice)), pkg.command, pkg.sequence, (char *)pkg.body);
        free(pkg.body);
        json_decref(params);
    }
    dict_release_iterator(iter);
}

int init_price(void)
{
    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function = dict_market_hash_func;
    dt.key_compare = dict_market_key_compare;
    dt.key_dup = dict_market_key_dup;
    dt.key_destructor = dict_market_key_free;
    dt.val_dup = dict_market_val_dup;
    dt.val_destructor = dict_market_val_free;

    dict_market = dict_create(&dt, 64);
    if (dict_market == NULL)
        return -__LINE__;

    rpc_clt_type ct;
    memset(&ct, 0, sizeof(ct));
    ct.on_connect = on_backend_connect;
    ct.on_recv_pkg = on_backend_recv_pkg;

    marketprice = rpc_clt_create(&settings.marketprice, &ct);
    if (marketprice == NULL)
        return -__LINE__;
    if (rpc_clt_start(marketprice) < 0)
        return -__LINE__;

    nw_state_type st;
    memset(&st, 0, sizeof(st));
    st.on_timeout = on_timeout;

    state_context = nw_state_create(&st, sizeof(struct state_data));
    if (state_context == NULL)
        return -__LINE__;

    nw_timer_set(&timer, settings.price_interval, true, on_timer, NULL);
    nw_timer_start(&timer);

    return 0;
}

int price_subscribe(nw_ses *ses, const char *market)
{
    dict_entry *entry = dict_find(dict_market, market);
    if (entry == NULL) {
        struct market_val val;
        memset(&val, 0, sizeof(val));

        dict_types dt;
        memset(&dt, 0, sizeof(dt));
        dt.hash_function = dict_ses_hash_func;
        dt.key_compare = dict_ses_key_compare;
        val.sessions = dict_create(&dt, 1024);
        if (val.sessions == NULL)
            return -__LINE__;

        val.last = mpd_new(&mpd_ctx);
        mpd_copy(val.last, mpd_zero, &mpd_ctx);

        entry = dict_add(dict_market, (char *)market, &val);
        if (entry == NULL)
            return -__LINE__;
    }

    struct market_val *obj = entry->val;
    dict_add(obj->sessions, ses, NULL);

    return 0;
}

int price_unsubscribe(nw_ses *ses)
{
    dict_iterator *iter = dict_get_iterator(dict_market);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct market_val *obj = entry->val;
        dict_delete(obj->sessions, ses);
    }
    dict_release_iterator(iter);

    return 0;
}

int price_send_last(nw_ses *ses, const char *market)
{
    dict_entry *entry = dict_find(dict_market, market);
    if (entry == NULL)
        return 0;
    struct market_val *obj = entry->val;
    if (mpd_cmp(obj->last, mpd_zero, &mpd_ctx) == 0)
        return 0;

    json_t *params = json_array();
    json_array_append_new(params, json_string(market));
    json_array_append_new_mpd(params, obj->last);
    send_notify(ses, "price.update", params);
    json_decref(params);

    return 0;
}

