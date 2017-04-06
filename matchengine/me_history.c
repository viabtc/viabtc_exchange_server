/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/06, create
 */

# include "me_config.h"
# include "me_history.h"

static nw_job *job;
static dict_t *dict_sql;
static nw_timer timer;

enum {
    HISTORY_USER_BALANCE,
    HISTORY_USER_ORDER,
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

static void dict_sql_val_free(void *val)
{
    sdsfree(val);
}

static void *on_job_init(void)
{
    return mysql_connect(&settings.db_log);
}

static void on_job(nw_job_entry *entry, void *privdata)
{
    MYSQL *conn = privdata;
    sds sql = entry->request;
    log_trace("exec sql: %s", sql);
    while (true) {
        int ret = mysql_real_query(conn, sql, sdslen(sql));
        if (ret != 0) {
            log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
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
    dict_iterator *iter = dict_get_iterator(dict_sql);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        sds sql = entry->val;
        if (sdslen(sql)) {
            nw_job_add(job, 0, sdsnewlen(sql, sdslen(sql)));
            sdsclear(sql);
        }
    }
}

int init_history(void)
{
    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function  = dict_sql_hash_function;
    dt.key_compare    = dict_sql_key_compare;
    dt.key_dup        = dict_sql_key_dup;
    dt.key_destructor = dict_sql_key_free;
    dt.val_destructor = dict_sql_val_free;

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

    job = nw_job_create(&jt, 20);
    if (job == NULL)
        return -__LINE__;

    nw_timer_set(&timer, 0.1, true, on_timer, NULL);
    nw_timer_start(&timer);

    return 0;
}

static int append_user_order(order_t *order)
{
    return 0;
}

static int append_order_detail(order_t *order)
{
    return 0;
}

static int append_user_balance(double t, uint32_t user_id, const char *asset, const char *business, mpd_t *change, mpd_t *balance, const char *detail)
{
    return 0;
}

static int append_order_deal(double t, uint64_t order_id, uint64_t deal_order_id, mpd_t *amount, mpd_t *price, mpd_t *deal, mpd_t *ask_fee, mpd_t *deal_fee)
{
    return 0;
}

int append_order_history(order_t *order)
{
    append_user_order(order);
    append_order_detail(order);
    return 0;
}

int append_user_balance_history(double t, uint32_t user_id, const char *asset, const char *business, mpd_t *change, mpd_t *balance, const char *detail)
{
    return append_user_balance(t, user_id, asset, business, change, balance, detail);
}

int append_order_deal_history(double t, uint64_t ask, uint64_t bid, mpd_t *amount, mpd_t *price, mpd_t *deal, mpd_t *ask_fee, mpd_t *deal_fee)
{
    append_order_detail(t, ask, bid, amount, price, deal, ask_fee);
    append_order_detail(t, bid, ask, amount, price, deal, bid_fee);
    return 0;
}

