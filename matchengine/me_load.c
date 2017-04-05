/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/04, create
 */

# include "me_config.h"
# include "me_load.h"
# include "me_log_load.h"

static time_t get_today_start(void)
{
    time_t now = time(NULL);
    struct tm *lt = localtime(&now);
    struct tm t;
    memset(&t, 0, sizeof(t));
    t.tm_year = lt->tm_year;
    t.tm_mon  = lt->tm_mon;
    t.tm_mday = lt->tm_mday;
    return mktime(&t);
}

static time_t get_last_slice(MYSQL *conn)
{
    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT YEAR(`end_date`), MONTH(`end_date`), DAY(`end_date`) from `slice_history` ORDER BY `end_date` DESC LIMIT 1");
    log_trace("exec sql: %s", sql);
    int ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret != 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        sdsfree(sql);
        return -__LINE__;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    size_t num_rows = mysql_num_rows(result);
    if (num_rows != 1) {
        mysql_free_result(result);
        return 0;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    struct tm t;
    memset(&t, 0, sizeof(t));
    t.tm_year = atoi(row[0]) - 1900;
    t.tm_mon  = atoi(row[1]) - 1;
    t.tm_mday = atoi(row[2]);

    return mktime(&t);
}

static int load_slice_from_db(MYSQL *conn, time_t start)
{
    struct tm *t = localtime(&start);
    sds table = sdsempty();
    table = sdscatprintf(table, "slice_balance_%04d%02d%02d", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
    int ret = load_balance(conn, table);
    if (ret < 0) {
        log_error("load_balance from %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    sdsclear(table);
    table = sdscatprintf(table, "slice_order_%04d%02d%02d", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
    ret = load_orders(conn, table);
    if (ret < 0) {
        log_error("load_orders from %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    return 0;
}

static int load_oper_log_from_db(MYSQL *conn, time_t start)
{
    struct tm *t = localtime(&start);
    sds table = sdsempty();
    table = sdscatprintf(table, "oper_log_%04d%02d%02d", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
    int ret = load_oper_log(conn, table);
    if (ret < 0) {
        log_error("load_oper_log from %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    return 0;
}

int init_from_db(void)
{
    MYSQL *conn = mysql_connect(&settings.db);
    if (conn == NULL)
        return -__LINE__;

    time_t today = get_today_start();
    time_t last_slice = get_last_slice(conn);
    if (last_slice == 0) {
        return load_oper_log_from_db(conn, today);
    }

    int ret = load_slice_from_db(conn, last_slice);
    if (ret < 0) {
        return ret;
    }

    time_t start = last_slice + 86400;
    while (start <= today) {
        int ret = load_oper_log_from_db(conn, start);
        if (ret < 0) {
            return ret;
        }
    }

    return 0;
}

