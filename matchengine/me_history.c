/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/06, create
 */

# include "me_config.h"
# include "me_history.h"
# include "me_balance.h"

static MYSQL *mysql_conn;
static bool use_mysql;
static bool use_postgresql;
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
    struct db_connection *conn = db_connect(&settings.db_history);
    return conn;
}

static void on_job(nw_job_entry *entry, void *privdata)
{
    struct db_connection *db = privdata;
    struct db_query *query = entry->request;
    if (is_need_insert_my(db)) {
        MYSQL *conn = db->mysql;
        sds sql = query->mysql;
        log_trace("exec mysql sql: %s", sql);
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
    if (is_need_insert_pg(db)) {
        // TODO add retry? if fail postgresql
        sds sql = query->postgresql;
        log_trace("exec postgresql sql: %s", sql);
        PGresult *res;
        res = PQexec(db->postgresql, sql);
        if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
            log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        }

        PQclear(res);
    }
}

static void on_job_cleanup(nw_job_entry *entry)
{
    struct db_query *query = entry->request;
    sdsfree(query->mysql);
    sdsfree(query->postgresql);
    free(entry->request);
}

static void on_job_release(void *privdata)
{
    struct db_connection *db = privdata;
    db_connection_free(db);
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
    use_mysql = false;
    use_postgresql = false;
    if (is_need_mysql(&settings.db_log)){
        use_mysql = true;
    }
    if (is_need_pgsql(&settings.db_log)){
        use_postgresql = true;
    }
    if (use_mysql) {
        mysql_conn = mysql_init(NULL);
        if (mysql_conn == NULL)
            return -__LINE__;
        if (mysql_options(mysql_conn, MYSQL_SET_CHARSET_NAME, settings.db_history.charset) != 0)
            return -__LINE__;
    }

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

static struct db_query  *get_query(struct dict_sql_key *key)
{
    // query's send to db periodic every 0.1 (sec?)
    // query's accumulate on dict_sql shard over table
    // dict_sql:  key aka (table_id(0...99), table_name code) => (sql_str)
    dict_entry *entry = dict_find(dict_sql, key);
    if (!entry) {
        struct db_query  *query = malloc(sizeof(struct db_query));
        query->mysql = sdsempty();
        query->postgresql = sdsempty();
        entry = dict_add(dict_sql, key, query);
        if (entry == NULL) {
            sdsfree(query->mysql);
            sdsfree(query->postgresql);
            free(query);
            return NULL;
        }
    }
    return entry->val;
}

static void set_query(struct dict_sql_key *key, struct db_query  *query)
{
    dict_entry *entry = dict_find(dict_sql, key);
    if (entry) {
        entry->val = query;
    }
}

static int append_user_order(order_t *order)
{
    struct dict_sql_key key;
    key.hash = order->user_id % HISTORY_HASH_NUM;
    key.type = HISTORY_USER_ORDER;
    struct db_query  *query = get_query(&key);
    if (query == NULL)
        return -__LINE__;

    if (use_mysql) {
        if (sdslen(query->mysql) == 0) {
            query->mysql = sdscatprintf(query->mysql, "INSERT INTO `order_history_%u` (`id`, `create_time`, `finish_time`, `user_id`, "
                                    "`market`, `source`, `t`, `side`, `price`, `amount`, `taker_fee`, `maker_fee`, `deal_stock`, `deal_money`, `deal_fee`) VALUES ",
                               key.hash);
        } else {
            query->mysql = sdscatprintf(query->mysql, ", ");
        }

        query->mysql = sdscatprintf(query->mysql, "(%"PRIu64", %f, %f, %u, '%s', '%s', %u, %u, ", order->id,
                           order->create_time, order->update_time, order->user_id, order->market, order->source,
                           order->type, order->side);
        query->mysql = sql_append_mpd(query->mysql, order->price, true);
        query->mysql = sql_append_mpd(query->mysql, order->amount, true);
        query->mysql = sql_append_mpd(query->mysql, order->taker_fee, true);
        query->mysql = sql_append_mpd(query->mysql, order->maker_fee, true);
        query->mysql = sql_append_mpd(query->mysql, order->deal_stock, true);
        query->mysql = sql_append_mpd(query->mysql, order->deal_money, true);
        query->mysql = sql_append_mpd(query->mysql, order->deal_fee, false);
        query->mysql = sdscatprintf(query->mysql, ")");
    }
    if (use_postgresql)
    {
        // TODO key.hash=user_id % 100 not need for postgresql because we inserted one table
        if (sdslen(query->postgresql) == 0) {
            query->postgresql = sdscat(query->postgresql, "INSERT INTO \"order_history\" (id, create_time, finish_time, user_id, market,"
                                    " source, order_type, side, price, amount , taker_fee, maker_fee, deal_stock, deal_money, deal_fee) VALUES ");
        } else {
            query->postgresql = sdscat(query->postgresql, ", ");
        }

        query->postgresql = sdscatprintf(query->postgresql, "(%"PRIu64", to_timestamp(%f), to_timestamp(%f), %u, '%s', '%s', %u, %u, ", order->id,
                           order->create_time, order->update_time, order->user_id, order->market, order->source,
                           order->type, order->side);
        query->postgresql = sql_append_mpd(query->postgresql, order->price, true);
        query->postgresql = sql_append_mpd(query->postgresql, order->amount, true);
        query->postgresql = sql_append_mpd(query->postgresql, order->taker_fee, true);
        query->postgresql = sql_append_mpd(query->postgresql, order->maker_fee, true);
        query->postgresql = sql_append_mpd(query->postgresql, order->deal_stock, true);
        query->postgresql = sql_append_mpd(query->postgresql, order->deal_money, true);
        query->postgresql = sql_append_mpd(query->postgresql, order->deal_fee, false);
        query->postgresql = sdscatprintf(query->postgresql, ")");
    }
    set_query(&key, query);

    return 0;
}

static int append_order_detail(order_t *order)
{
    struct dict_sql_key key;
    key.hash = order->id % HISTORY_HASH_NUM;
    key.type = HISTORY_ORDER_DETAIL;
    struct db_query  *query = get_query(&key);
    if (query == NULL)
        return -__LINE__;
    if (use_mysql) {
        if (sdslen(query->mysql) == 0) {
            query->mysql = sdscatprintf(query->mysql, "INSERT INTO `order_detail_%u` (`id`, `create_time`, `finish_time`, `user_id`, "
                                    "`market`, `source`, `t`, `side`, `price`, `amount`, `taker_fee`, `maker_fee`, `deal_stock`, `deal_money`, `deal_fee`) VALUES ",
                               key.hash);
        } else {
            query->mysql = sdscatprintf(query->mysql, ", ");
        }

        query->mysql = sdscatprintf(query->mysql, "(%"PRIu64", %f, %f, %u, '%s', '%s', %u, %u, ", order->id,
                           order->create_time, order->update_time, order->user_id, order->market, order->source,
                           order->type, order->side);
        query->mysql = sql_append_mpd(query->mysql, order->price, true);
        query->mysql = sql_append_mpd(query->mysql, order->amount, true);
        query->mysql = sql_append_mpd(query->mysql, order->taker_fee, true);
        query->mysql = sql_append_mpd(query->mysql, order->maker_fee, true);
        query->mysql = sql_append_mpd(query->mysql, order->deal_stock, true);
        query->mysql = sql_append_mpd(query->mysql, order->deal_money, true);
        query->mysql = sql_append_mpd(query->mysql, order->deal_fee, false);
        query->mysql = sdscatprintf(query->mysql, ")");

    }
    if (use_postgresql)
    {
        // TODO key.hash=id % 100 not need for postgresql because we inserted one table
        if (sdslen(query->postgresql) == 0) {
            query->postgresql = sdscat(query->postgresql, "INSERT INTO \"order_detail\" (id, create_time, finish_time, user_id, market,"
                                                          " source, order_type, side, price, amount , taker_fee, maker_fee, deal_stock, deal_money, deal_fee) VALUES ");
        } else {
            query->postgresql = sdscat(query->postgresql, ", ");
        }

        query->postgresql = sdscatprintf(query->postgresql, "(%"PRIu64", to_timestamp(%f), to_timestamp(%f), %u, '%s', '%s', %u, %u, ", order->id,
                                         order->create_time, order->update_time, order->user_id, order->market, order->source,
                                         order->type, order->side);
        query->postgresql = sql_append_mpd(query->postgresql, order->price, true);
        query->postgresql = sql_append_mpd(query->postgresql, order->amount, true);
        query->postgresql = sql_append_mpd(query->postgresql, order->taker_fee, true);
        query->postgresql = sql_append_mpd(query->postgresql, order->maker_fee, true);
        query->postgresql = sql_append_mpd(query->postgresql, order->deal_stock, true);
        query->postgresql = sql_append_mpd(query->postgresql, order->deal_money, true);
        query->postgresql = sql_append_mpd(query->postgresql, order->deal_fee, false);
        query->postgresql = sdscatprintf(query->postgresql, ")");
    }
    set_query(&key, query);

    return 0;
}

static int append_order_deal(double t, uint32_t user_id, uint64_t deal_id, uint64_t order_id, uint64_t deal_order_id, int role, mpd_t *price, mpd_t *amount, mpd_t *deal, mpd_t *fee, mpd_t *deal_fee)
{
    struct dict_sql_key key;
    key.hash = order_id % HISTORY_HASH_NUM;
    key.type = HISTORY_ORDER_DEAL;
    struct db_query  *query = get_query(&key);
    if (query == NULL)
        return -__LINE__;
    if (use_mysql) {
        if (sdslen(query->mysql) == 0) {
            query->mysql = sdscatprintf(query->mysql,
                               "INSERT INTO `deal_history_%u` (`id`, `time`, `user_id`, `deal_id`, `order_id`, `deal_order_id`,"
                               " `role`, `price`, `amount`, `deal`, `fee`, `deal_fee`) VALUES ",
                               key.hash);
        } else {
            query->mysql = sdscatprintf(query->mysql, ", ");
        }

        query->mysql = sdscatprintf(query->mysql, "(NULL, %f, %u, %"PRIu64", %"PRIu64", %"PRIu64", %d, ", t, user_id, deal_id, order_id,
                           deal_order_id, role);
        query->mysql = sql_append_mpd(query->mysql, price, true);
        query->mysql = sql_append_mpd(query->mysql, amount, true);
        query->mysql = sql_append_mpd(query->mysql, deal, true);
        query->mysql = sql_append_mpd(query->mysql, fee, true);
        query->mysql = sql_append_mpd(query->mysql, deal_fee, false);
        query->mysql = sdscatprintf(query->mysql, ")");
    }
    if (use_postgresql)
    {
        // TODO rm key.hash=order_id % 100 not need for postgresql because we inserted one table
        if (sdslen(query->postgresql) == 0) {
            query->postgresql = sdscat(query->postgresql, "INSERT INTO \"deal_history\" (time, user_id, deal_id,"
                                                          " order_id, deal_order_id, role, price, amount , deal, "
                                                          "fee, deal_fee"
                                                          ") VALUES ");
        } else {
            query->postgresql = sdscat(query->postgresql, ", ");
        }

        query->postgresql = sdscatprintf(query->postgresql, "(to_timestamp(%f), %u, %"PRIu64", %"PRIu64", %"PRIu64", %d, ", t, user_id, deal_id, order_id,
                           deal_order_id, role);
        query->postgresql = sql_append_mpd(query->postgresql, price, true);
        query->postgresql = sql_append_mpd(query->postgresql, amount, true);
        query->postgresql = sql_append_mpd(query->postgresql, deal, true);
        query->postgresql = sql_append_mpd(query->postgresql, fee, true);
        query->postgresql = sql_append_mpd(query->postgresql, deal_fee, false);
        query->postgresql = sdscatprintf(query->postgresql, ")");
    }
    set_query(&key, query);

    return 0;
}

static int append_user_deal(double t, uint32_t user_id, const char *market, uint64_t deal_id, uint64_t order_id, uint64_t deal_order_id, int side, int role, mpd_t *price, mpd_t *amount, mpd_t *deal, mpd_t *fee, mpd_t *deal_fee)
{
    struct dict_sql_key key;
    key.hash = user_id % HISTORY_HASH_NUM;
    key.type = HISTORY_USER_DEAL;
    struct db_query  *query = get_query(&key);
    if (query == NULL)
        return -__LINE__;
    if (use_mysql) {
        if (sdslen(query->mysql) == 0) {
            query->mysql = sdscatprintf(query->mysql,
                               "INSERT INTO `user_deal_history_%u` (`id`, `time`, `user_id`, `market`, `deal_id`, `order_id`, `deal_order_id`, `side`, `role`, `price`, `amount`, `deal`, `fee`, `deal_fee`) VALUES ",
                               key.hash);
        } else {
            query->mysql = sdscatprintf(query->mysql, ", ");
        }

        query->mysql = sdscatprintf(query->mysql, "(NULL, %f, %u, '%s', %"PRIu64", %"PRIu64", %"PRIu64", %d, %d, ", t, user_id, market,
                           deal_id, order_id, deal_order_id, side, role);
        query->mysql = sql_append_mpd(query->mysql, price, true);
        query->mysql = sql_append_mpd(query->mysql, amount, true);
        query->mysql = sql_append_mpd(query->mysql, deal, true);
        query->mysql = sql_append_mpd(query->mysql, fee, true);
        query->mysql = sql_append_mpd(query->mysql, deal_fee, false);
        query->mysql = sdscatprintf(query->mysql, ")");
    }
    if (use_postgresql)
    {
        // TODO rm key.hash=user_id % 100 not need for postgresql because we inserted one table
        if (sdslen(query->postgresql) == 0) {
            query->postgresql = sdscat(query->postgresql,
                               "INSERT INTO \"user_deal_history\" (\"time\", user_id, market, deal_id, order_id, "
                               "deal_order_id, side, role, price, amount, deal, fee, deal_fee) VALUES ");
        } else {
            query->postgresql = sdscat(query->postgresql, ", ");
        }

        query->postgresql = sdscatprintf(query->postgresql, "(to_timestamp(%f), %u, '%s', %"PRIu64", %"PRIu64", %"PRIu64", %d, %d, ",
                t, user_id, market, deal_id, order_id, deal_order_id, side, role);
        query->postgresql = sql_append_mpd(query->postgresql, price, true);
        query->postgresql = sql_append_mpd(query->postgresql, amount, true);
        query->postgresql = sql_append_mpd(query->postgresql, deal, true);
        query->postgresql = sql_append_mpd(query->postgresql, fee, true);
        query->postgresql = sql_append_mpd(query->postgresql, deal_fee, false);
        query->postgresql = sdscatprintf(query->postgresql, ")");
    }
    set_query(&key, query);

    return 0;
}

static int append_user_balance(double t, uint32_t user_id, const char *asset, const char *business, mpd_t *change, mpd_t *balance, const char *detail)
{
    struct dict_sql_key key;
    key.hash = user_id % HISTORY_HASH_NUM;
    key.type = HISTORY_USER_BALANCE;
    struct db_query  *query = get_query(&key);
    if (query == NULL)
        return -__LINE__;
    if (use_mysql) {
        if (sdslen(query->mysql) == 0) {
            query->mysql = sdscatprintf(query->mysql,
                               "INSERT INTO `balance_history_%u` (`id`, `time`, `user_id`, `asset`, `business`, `change`, `balance`, `detail`) VALUES ",
                               key.hash);
        } else {
            query->mysql = sdscatprintf(query->mysql, ", ");
        }

        char buf[10 * 1024];
        query->mysql = sdscatprintf(query->mysql, "(NULL, %f, %u, '%s', '%s', ", t, user_id, asset, business);
        query->mysql = sql_append_mpd(query->mysql, change, true);
        query->mysql = sql_append_mpd(query->mysql, balance, true);
        mysql_real_escape_string(mysql_conn, buf, detail, strlen(detail));
        query->mysql = sdscatprintf(query->mysql, "'%s')", buf);
    }
    if (use_postgresql)
    {
        // TODO rm key.hash=user_id % 100 not need for postgresql because we inserted one table
        if (sdslen(query->postgresql) == 0) {
            query->postgresql = sdscat(query->postgresql, "INSERT INTO \"balance_history\" (\"time\", user_id, asset, business, change, balance, detail) VALUES ");
        } else {
            query->postgresql = sdscat(query->postgresql, ", ");
        }
        query->postgresql = sdscatprintf(query->postgresql, "(to_timestamp(%f), %u, '%s', '%s', ", t, user_id, asset, business);
        query->postgresql = sql_append_mpd(query->postgresql, change, true);
        query->postgresql = sql_append_mpd(query->postgresql, balance, true);
        // TODO  need escape_string
        query->postgresql = sdscatprintf(query->postgresql, "'%s')", detail);
    }
    set_query(&key, query);

    return 0;
}

static int append_wallet_balance_fee(order_t *order, const char *stock, const char *money)
{
    struct dict_sql_key key;
    key.hash = 0;
    key.type = HISTORY_EXCHANGE_DATA;
    struct db_query  *query = get_query(&key);
    if (query == NULL)
        return -__LINE__;
    if (use_mysql) {
        if (sdslen(query->mysql) == 0) {
            query->mysql = sdscatprintf(query->mysql,
                               "INSERT INTO `exchange_data` (`id`, `time`, `asset`, `business`, `change`) VALUES ");
        } else {
            query->mysql = sdscatprintf(query->mysql, ", ");
        }

        query->mysql = sdscatprintf(query->mysql, "(NULL, %f, ", order->update_time);
        if (order->side == 1) {
            query->mysql = sdscatprintf(query->mysql, "'%s', ", money);
        } else {
            query->mysql = sdscatprintf(query->mysql, "'%s', ", stock);
        }

        query->mysql = sdscatprintf(query->mysql, "'fee', ");
        query->mysql = sql_append_mpd(query->mysql, order->deal_fee, false);
        query->mysql = sdscatprintf(query->mysql, ")");
    }
    if (use_postgresql)
    {
        if (sdslen(query->postgresql) == 0) {
            query->postgresql = sdscatprintf(query->postgresql,
                                        "INSERT INTO \"exchange_data\" (\"time\", order_id, user_id, asset,"
                                        " operation, amount) VALUES ");
        } else {
            query->postgresql = sdscat(query->postgresql, ", ");
        }

        query->postgresql = sdscatprintf(query->postgresql, "(to_timestamp(%f),%"PRIu64",%u, ",
                order->update_time, order->id, order->user_id);

        if (order->side == 1) {
            query->postgresql = sdscatprintf(query->postgresql, "'%s', ", money);
        } else {
            query->postgresql = sdscatprintf(query->postgresql, "'%s', ", stock);
        }

        query->postgresql = sdscat(query->postgresql, "'fee', ");
        query->postgresql = sql_append_mpd(query->postgresql, order->deal_fee, false);
        query->postgresql = sdscatprintf(query->postgresql, ")");
    }
    set_query(&key, query);

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

