/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/06, create
 */

# include "me_config.h"
# include "me_history.h"
# include "me_balance.h"

static MYSQL *mysql_conn;
static nw_job *job;
static dict_t *dict_sql;
static nw_timer timer;

enum {
    HISTORY_USER_BALANCE,
    HISTORY_USER_ORDER,
    HISTORY_USER_DEAL,
    HISTORY_ORDER_DETAIL,
    HISTORY_ORDER_DEAL,
};

struct dict_sql_key {
    uint32_t type;
    uint32_t hash;
};

static uint32_t dict_sql_hash_function(const void *key)
{
    return dict_generic_hash_function(key, sizeof(struct dict_sql_key));
}

static void *dict_sql_key_dup(const void *key)
{
    struct dict_sql_key *obj = malloc(sizeof(struct dict_sql_key));
    memcpy(obj, key, sizeof(struct dict_sql_key));
    return obj;
}

static int dict_sql_key_compare(const void *key1, const void *key2)
{
    return memcmp(key1, key2, sizeof(struct dict_sql_key));
}

static void dict_sql_key_free(void *key)
{
    free(key);
}

static void *on_job_init(void)
{
    return mysql_connect(&settings.db_history);
}

static void on_job(nw_job_entry *entry, void *privdata)
{
    MYSQL *conn = privdata;
    sds sql = entry->request;
    log_trace("exec sql: %s", sql);
    while (true) {
        int ret = mysql_real_query(conn, sql, sdslen(sql));
        if (ret != 0 && mysql_errno(conn) != 1062) {
            log_fatal("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
            usleep(1000 * 1000);
            continue;
        }
        break;
    }
}

static void on_job_cleanup(nw_job_entry *entry)
{
    sdsfree(entry->request);
}

static void on_job_release(void *privdata)
{
    mysql_close(privdata);
}

static void on_timer(nw_timer *t, void *privdata)
{
    size_t count = 0;
    dict_iterator *iter = dict_get_iterator(dict_sql);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        nw_job_add(job, 0, entry->val);
        dict_delete(dict_sql, entry->key);
        count++;
    }
    dict_release_iterator(iter);

    if (count) {
        log_debug("flush history count: %zu", count);
    }
}

int init_history(void)
{
    mysql_conn = mysql_init(NULL);
    if (mysql_conn == NULL)
        return -__LINE__;
    if (mysql_options(mysql_conn, MYSQL_SET_CHARSET_NAME, settings.db_history.charset) != 0)
        return -__LINE__;

    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function  = dict_sql_hash_function;
    dt.key_compare    = dict_sql_key_compare;
    dt.key_dup        = dict_sql_key_dup;
    dt.key_destructor = dict_sql_key_free;

    dict_sql = dict_create(&dt, 1024);
    if (dict_sql == 0) {
        return -__LINE__;
    }

    nw_job_type jt;
    memset(&jt, 0, sizeof(jt));
    jt.on_init    = on_job_init;
    jt.on_job     = on_job;
    jt.on_cleanup = on_job_cleanup;
    jt.on_release = on_job_release;

    job = nw_job_create(&jt, settings.history_thread);
    if (job == NULL)
        return -__LINE__;

    nw_timer_set(&timer, 0.1, true, on_timer, NULL);
    nw_timer_start(&timer);

    return 0;
}

int fini_history(void)
{
    on_timer(NULL, NULL);

    usleep(100 * 1000);
    nw_job_release(job);

    return 0;
}

static sds sql_append_mpd(sds sql, mpd_t *val, bool comma)
{
    char *str = mpd_to_sci(val, 0);
    sql = sdscatprintf(sql, "'%s'", str);
    if (comma) {
        sql = sdscatprintf(sql, ", ");
    }
    free(str);
    return sql;
}

static sds get_sql(struct dict_sql_key *key)
{
    dict_entry *entry = dict_find(dict_sql, key);
    if (!entry) {
        sds val = sdsempty();
        entry = dict_add(dict_sql, key, val);
        if (entry == NULL) {
            sdsfree(val);
            return NULL;
        }
    }
    return entry->val;
}

static void set_sql(struct dict_sql_key *key, sds sql)
{
    dict_entry *entry = dict_find(dict_sql, key);
    if (entry) {
        entry->val = sql;
    }
}

static int append_user_order(order_t *order)
{
    struct dict_sql_key key;
    key.hash = order->user_id % HISTORY_HASH_NUM;
    key.type = HISTORY_USER_ORDER;
    sds sql = get_sql(&key);
    if (sql == NULL)
        return -__LINE__;

    if (sdslen(sql) == 0) {
        sql = sdscatprintf(sql, "INSERT INTO `order_history_%u` (`id`, `create_time`, `finish_time`, `user_id`, "
                "`market`, `source`, `t`, `side`, `price`, `amount`, `taker_fee`, `maker_fee`, `deal_stock`, `deal_money`, `deal_fee`) VALUES ", key.hash);
    } else {
        sql = sdscatprintf(sql, ", ");
    }

    sql = sdscatprintf(sql, "(%"PRIu64", %f, %f, %u, '%s', '%s', %u, %u, ", order->id,
        order->create_time, order->update_time, order->user_id, order->market, order->source, order->type, order->side);
    sql = sql_append_mpd(sql, order->price, true);
    sql = sql_append_mpd(sql, order->amount, true);
    sql = sql_append_mpd(sql, order->taker_fee, true);
    sql = sql_append_mpd(sql, order->maker_fee, true);
    sql = sql_append_mpd(sql, order->deal_stock, true);
    sql = sql_append_mpd(sql, order->deal_money, true);
    sql = sql_append_mpd(sql, order->deal_fee, false);
    sql = sdscatprintf(sql, ")");

    set_sql(&key, sql);

    return 0;
}

static int append_order_detail(order_t *order)
{
    struct dict_sql_key key;
    key.hash = order->id % HISTORY_HASH_NUM;
    key.type = HISTORY_ORDER_DETAIL;
    sds sql = get_sql(&key);
    if (sql == NULL)
        return -__LINE__;

    if (sdslen(sql) == 0) {
        sql = sdscatprintf(sql, "INSERT INTO `order_detail_%u` (`id`, `create_time`, `finish_time`, `user_id`, "
                "`market`, `source`, `t`, `side`, `price`, `amount`, `taker_fee`, `maker_fee`, `deal_stock`, `deal_money`, `deal_fee`) VALUES ", key.hash);
    } else {
        sql = sdscatprintf(sql, ", ");
    }

    sql = sdscatprintf(sql, "(%"PRIu64", %f, %f, %u, '%s', '%s', %u, %u, ", order->id,
        order->create_time, order->update_time, order->user_id, order->market, order->source, order->type, order->side);
    sql = sql_append_mpd(sql, order->price, true);
    sql = sql_append_mpd(sql, order->amount, true);
    sql = sql_append_mpd(sql, order->taker_fee, true);
    sql = sql_append_mpd(sql, order->maker_fee, true);
    sql = sql_append_mpd(sql, order->deal_stock, true);
    sql = sql_append_mpd(sql, order->deal_money, true);
    sql = sql_append_mpd(sql, order->deal_fee, false);
    sql = sdscatprintf(sql, ")");

    set_sql(&key, sql);

    return 0;
}

static int append_order_deal(double t, uint32_t user_id, uint64_t deal_id, uint64_t order_id, uint64_t deal_order_id, int role, mpd_t *price, mpd_t *amount, mpd_t *deal, mpd_t *fee, mpd_t *deal_fee)
{
    struct dict_sql_key key;
    key.hash = order_id % HISTORY_HASH_NUM;
    key.type = HISTORY_ORDER_DEAL;
    sds sql = get_sql(&key);
    if (sql == NULL)
        return -__LINE__;

    if (sdslen(sql) == 0) {
        sql = sdscatprintf(sql, "INSERT INTO `deal_history_%u` (`id`, `time`, `user_id`, `deal_id`, `order_id`, `deal_order_id`, `role`, `price`, `amount`, `deal`, `fee`, `deal_fee`) VALUES ", key.hash);
    } else {
        sql = sdscatprintf(sql, ", ");
    }

    sql = sdscatprintf(sql, "(NULL, %f, %u, %"PRIu64", %"PRIu64", %"PRIu64", %d, ", t, user_id, deal_id, order_id, deal_order_id, role);
    sql = sql_append_mpd(sql, price, true);
    sql = sql_append_mpd(sql, amount, true);
    sql = sql_append_mpd(sql, deal, true);
    sql = sql_append_mpd(sql, fee, true);
    sql = sql_append_mpd(sql, deal_fee, false);
    sql = sdscatprintf(sql, ")");

    set_sql(&key, sql);

    return 0;
}

static int append_user_deal(double t, uint32_t user_id, const char *market, uint64_t deal_id, uint64_t order_id, uint64_t deal_order_id, int side, int role, mpd_t *price, mpd_t *amount, mpd_t *deal, mpd_t *fee, mpd_t *deal_fee)
{
    struct dict_sql_key key;
    key.hash = user_id % HISTORY_HASH_NUM;
    key.type = HISTORY_USER_DEAL;
    sds sql = get_sql(&key);
    if (sql == NULL)
        return -__LINE__;

    if (sdslen(sql) == 0) {
        sql = sdscatprintf(sql, "INSERT INTO `user_deal_history_%u` (`id`, `time`, `user_id`, `market`, `deal_id`, `order_id`, `deal_order_id`, `side`, `role`, `price`, `amount`, `deal`, `fee`, `deal_fee`) VALUES ", key.hash);
    } else {
        sql = sdscatprintf(sql, ", ");
    }

    sql = sdscatprintf(sql, "(NULL, %f, %u, '%s', %"PRIu64", %"PRIu64", %"PRIu64", %d, %d, ", t, user_id, market, deal_id, order_id, deal_order_id, side, role);
    sql = sql_append_mpd(sql, price, true);
    sql = sql_append_mpd(sql, amount, true);
    sql = sql_append_mpd(sql, deal, true);
    sql = sql_append_mpd(sql, fee, true);
    sql = sql_append_mpd(sql, deal_fee, false);
    sql = sdscatprintf(sql, ")");

    set_sql(&key, sql);

    return 0;
}

static int append_user_balance(double t, uint32_t user_id, const char *asset, const char *business, mpd_t *change, mpd_t *balance, const char *detail)
{
    struct dict_sql_key key;
    key.hash = user_id % HISTORY_HASH_NUM;
    key.type = HISTORY_USER_BALANCE;
    sds sql = get_sql(&key);
    if (sql == NULL)
        return -__LINE__;

    if (sdslen(sql) == 0) {
        sql = sdscatprintf(sql, "INSERT INTO `balance_history_%u` (`id`, `time`, `user_id`, `asset`, `business`, `change`, `balance`, `detail`) VALUES ", key.hash);
    } else {
        sql = sdscatprintf(sql, ", ");
    }

    char buf[10 * 1024];
    sql = sdscatprintf(sql, "(NULL, %f, %u, '%s', '%s', ", t, user_id, asset, business);
    sql = sql_append_mpd(sql, change, true);
    sql = sql_append_mpd(sql, balance, true);
    mysql_real_escape_string(mysql_conn, buf, detail, strlen(detail));
    sql = sdscatprintf(sql, "'%s')", buf);

    set_sql(&key, sql);

    return 0;
}

int append_order_history(order_t *order)
{
    append_user_order(order);
    append_order_detail(order);

    return 0;
}

int append_order_deal_history(double t, uint64_t deal_id, order_t *ask, int ask_role, order_t *bid, int bid_role, mpd_t *price, mpd_t *amount, mpd_t *deal, mpd_t *ask_fee, mpd_t *bid_fee)
{
    append_order_deal(t, ask->user_id, deal_id, ask->id, bid->id, ask_role, price, amount, deal, ask_fee, bid_fee);
    append_order_deal(t, bid->user_id, deal_id, bid->id, ask->id, bid_role, price, amount, deal, bid_fee, ask_fee);

    append_user_deal(t, ask->user_id, ask->market, deal_id, ask->id, bid->id, ask->side, ask_role, price, amount, deal, ask_fee, bid_fee);
    append_user_deal(t, bid->user_id, ask->market, deal_id, bid->id, ask->id, bid->side, bid_role, price, amount, deal, bid_fee, ask_fee);

    return 0;
}

int append_user_balance_history(double t, uint32_t user_id, const char *asset, const char *business, mpd_t *change, const char *detail)
{
    mpd_t *balance = balance_total(user_id, asset);
    append_user_balance(t, user_id, asset, business, change, balance, detail);
    mpd_del(balance);

    return 0;
}

bool is_history_block(void)
{
    if (job->request_count >= MAX_PENDING_HISTORY) {
        return true;
    }
    return false;
}

sds history_status(sds reply)
{
    return sdscatprintf(reply, "history pending %d\n", job->request_count);
}

