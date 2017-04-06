/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/04, create
 */

# include "me_config.h"
# include "me_persist.h"
# include "me_load.h"
# include "me_dump.h"

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
    log_stderr("get last slice date");
    int ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret != 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        log_stderr("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
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

    table = sdscatprintf(table, "slice_order_%04d%02d%02d", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
    log_stderr("load orders from: %s", table);
    int ret = load_orders(conn, table);
    if (ret < 0) {
        log_error("load_orders from %s fail: %d", table, ret);
        log_stderr("load_orders from %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    sdsclear(table);
    table = sdscatprintf(table, "slice_market_%04d%02d%02d", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
    log_stderr("load market from: %s", table);
    ret = load_markets(conn, table);
    if (ret < 0) {
        log_error("load_markets from %s fail: %d", table, ret);
        log_stderr("load_markets from %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    sdsclear(table);
    table = sdscatprintf(table, "slice_balance_%04d%02d%02d", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
    log_stderr("load balance from: %s", table);
    ret = load_balance(conn, table);
    if (ret < 0) {
        log_error("load_markets from %s fail: %d", table, ret);
        log_stderr("load_balance from %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    sdsfree(table);
    return 0;
}

static int load_oper_log_from_db(MYSQL *conn, time_t start)
{
    struct tm *t = localtime(&start);
    sds table = sdsempty();
    table = sdscatprintf(table, "oper_log_%04d%02d%02d", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
    log_stderr("load oper log from: %s", table);
    if (!is_table_exists(conn, table)) {
        log_error("table %s not exist", table);
        log_stderr("table %s not exist", table);
        sdsfree(table);
        return 0;
    }

    int ret = load_oper_log(conn, table);
    if (ret < 0) {
        log_error("load_oper_log from %s fail: %d", table, ret);
        log_stderr("load_oper_log from %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    sdsfree(table);
    return 0;
}

int init_from_db(void)
{
    MYSQL *conn = mysql_connect(&settings.db_log);
    if (conn == NULL) {
        log_error("connect mysql fail");
        log_stderr("connect mysql fail");
        return -__LINE__;
    }

    int ret = 0;
    time_t today = get_today_start();
    time_t last_slice = get_last_slice(conn);
    if (last_slice == 0) {
        ret = load_oper_log_from_db(conn, today);
        if (ret < 0)
            goto cleanup;
    }

    ret = load_slice_from_db(conn, last_slice);
    if (ret < 0) {
        goto cleanup;
    }

    time_t start = last_slice + 86400;
    while (start <= today) {
        ret = load_oper_log_from_db(conn, start);
        if (ret < 0) {
            goto cleanup;
        }
        start += 86400;
    }

    log_stderr("load success");
    return 0;

cleanup:
    mysql_close(conn);
    return ret;
}

static int dump_order_to_db(MYSQL *conn, time_t start)
{
    struct tm *t = localtime(&start);
    sds table = sdsempty();
    table = sdscatprintf(table, "slice_order_%04d%02d%02d", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
    log_stderr("dump order to: %s", table);
    int ret = dump_orders(conn, table);
    if (ret < 0) {
        log_error("dump_orders to %s fail: %d", table, ret);
        log_stderr("dump_orders to %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    sdsfree(table);
    return 0;
}

static int dump_market_to_db(MYSQL *conn, time_t start)
{
    struct tm *t = localtime(&start);
    sds table = sdsempty();
    table = sdscatprintf(table, "slice_market_%04d%02d%02d", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
    log_stderr("dump market to: %s", table);
    int ret = dump_markets(conn, table);
    if (ret < 0) {
        log_error("dump_markets to %s fail: %d", table, ret);
        log_stderr("dump_markets to %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    sdsfree(table);
    return 0;
}

static int dump_balance_to_db(MYSQL *conn, time_t start)
{
    struct tm *t = localtime(&start);
    sds table = sdsempty();
    table = sdscatprintf(table, "slice_balance_%04d%02d%02d", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
    log_stderr("dump balance to: %s", table);
    int ret = dump_balance(conn, table);
    if (ret < 0) {
        log_error("dump_balance to %s fail: %d", table, ret);
        log_stderr("dump_balance to %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    sdsfree(table);
    return 0;
}

int update_slice_history(MYSQL *conn, time_t start)
{
    struct tm *t = localtime(&start);
    time_t now = time(NULL);
    sds sql = sdsempty();
    sql = sdscatprintf(sql, "INSERT INTO `slice_history` (`id`, `time`, `end_date`) VALUES (NULL, %ld, '%04d-%02d-%02d')",
            now, 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
    log_trace("exec sql: %s", sql);
    log_stderr("update slice history");
    int ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret < 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        log_stderr("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        sdsfree(sql);
        return -__LINE__;
    }

    sdsfree(sql);
    return 0;
}

int dump_to_db(void)
{
    MYSQL *conn = mysql_connect(&settings.db_log);
    if (conn == NULL) {
        log_error("connect mysql fail");
        log_stderr("connect mysql fail");
        return -__LINE__;
    }

    time_t yestarday = get_today_start() - 86400;
    time_t last_slice = get_last_slice(conn);
    if (last_slice == yestarday) {
        log_stderr("aleady dump");
        return 0;
    }

    int ret;
    if (last_slice == 0) {
        ret = load_oper_log_from_db(conn, yestarday);
        if (ret < 0) {
            goto cleanup;
        }
    } else {
        ret = load_slice_from_db(conn, last_slice);
        if (ret < 0) {
            goto cleanup;
        }

        time_t start = last_slice + 86400;
        while (start <= yestarday) {
            ret = load_oper_log_from_db(conn, start);
            if (ret < 0) {
                goto cleanup;
            }
            start += 86400;
        }
    }

    ret = dump_order_to_db(conn, yestarday);
    if (ret < 0) {
        goto cleanup;
    }

    ret = dump_market_to_db(conn, yestarday);
    if (ret < 0) {
        goto cleanup;
    }

    ret = dump_balance_to_db(conn, yestarday);
    if (ret < 0) {
        goto cleanup;
    }

    ret = update_slice_history(conn, yestarday);
    if (ret < 0) {
        goto cleanup;
    }

    log_stderr("dump success");
    return 0;

cleanup:
    mysql_close(conn);
    return ret;
}

