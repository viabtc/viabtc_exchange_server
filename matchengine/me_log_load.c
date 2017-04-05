/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/04, create
 */

# include "ut_mysql.h"
# include "me_trade.h"
# include "me_market.h"
# include "me_balance.h"

static int get_count_from_table(MYSQL *conn, const char *table)
{
    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT COUNT(*) from `%s`", table);
    log_trace("exec sql: %s", sql);
    int ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret != 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        sdsfree(sql);
        return -__LINE__;
    }
    sdsfree(sql);

    MYSQL_RES *result = mysql_store_result(conn);
    size_t num_rows = mysql_num_rows(result);
    if (num_rows != 1) {
        mysql_free_result(result);
        return -__LINE__;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int count = atoi(row[0]);
    mysql_free_result(result);

    return count;
}

static int do_load_orders(MYSQL *conn, const char *table)
{
    int count = get_count_from_table(conn, table);
    if (count < 0)
        return count;

    size_t query_limit = 1000;
    uint64_t last_id = 0;
    while (true) {
        sds sql = sdsempty();
        sql = sdscatprintf(sql, "SELECT `id`, `t`, `side`, `create_time`, `update_time`, `user_id`, `market`, "
                "`price`, `amount`, `fee`, `left`, `freeze`, `deal_stock`, `deal_money`, `deal_fee` FROM `%s` "
                "WHERE `id` > %"PRIu64" ORDER BY `id` LIMIT %zu", table, last_id, query_limit);
        log_trace("exec sql: %s", sql);
        int ret = mysql_real_query(conn, sql, sdslen(sql));
        if (ret != 0) {
            log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
            sdsfree(sql);
            return -__LINE__;
        }
        sdsfree(sql);

        MYSQL_RES *result = mysql_store_result(conn);
        size_t num_rows = mysql_num_rows(result);
        for (size_t i = 0; i < num_rows; ++i) {
            MYSQL_ROW row = mysql_fetch_row(result);
            market_t *market = get_market(row[6]);
            if (market == NULL)
                continue;

            order_t *order = malloc(sizeof(order_t));
            memset(order, 0, sizeof(order_t));
            order->id = strtoull(row[0], NULL, 0);
            order->type = strtoul(row[1], NULL, 0);
            order->side = strtoul(row[2], NULL, 0);
            order->create_time = strtod(row[3], NULL);
            order->update_time = strtod(row[4], NULL);
            order->user_id = strtoul(row[5], NULL, 0);
            order->market = strdup(row[6]);
            order->price = decimal(row[7], market->money_prec);
            order->amount = decimal(row[8], market->stock_prec);
            order->fee = decimal(row[9], market->fee_prec);
            order->left = decimal(row[10], market->stock_prec);
            order->freeze = decimal(row[11], 0);
            order->deal_stock = decimal(row[12], 0);
            order->deal_money = decimal(row[13], 0);
            order->deal_fee = decimal(row[14], 0);

            market_put_order(market, order);
        }
        mysql_free_result(result);

        if (num_rows < query_limit)
            break;
    }

    return 0;
}

static int do_load_balance(MYSQL *conn, const char *table)
{
    int count = get_count_from_table(conn, table);
    if (count < 0)
        return count;

    return 0;
}

static int do_load_oper_log(MYSQL *conn, const char *table)
{
    int count = get_count_from_table(conn, table);
    if (count < 0)
        return count;

    return 0;
}

int load_orders(const char *table)
{
    MYSQL *conn = mysql_connect(&settings.db);
    if (conn == NULL)
        return -__LINE__;
    int ret = do_load_orders(conn, table);
    if (ret < 0) {
        log_error("do_load_orders table: %s fail: %d", table, ret);
        mysql_close(conn);
        return -__LINE__;
    }

    mysql_close(conn);
    return 0;
}

int load_balance(const char *table)
{
    MYSQL *conn = mysql_connect(&settings.db);
    if (conn == NULL)
        return -__LINE__;
    int ret = do_load_balance(conn, table);
    if (ret < 0) {
        log_error("do_load_balance table: %s fail: %d", table, ret);
        mysql_close(conn);
        return -__LINE__;
    }

    mysql_close(conn);
    return 0;
}

int load_oper_log(const char *table)
{
    MYSQL *conn = mysql_connect(&settings.db);
    if (conn == NULL)
        return -__LINE__;
    int ret = do_load_oper_log(conn, table);
    if (ret < 0) {
        log_error("do_load_oper_log table: %s fail: %d", table, ret);
        mysql_close(conn);
        return -__LINE__;
    }

    mysql_close(conn);
    return 0;
}

