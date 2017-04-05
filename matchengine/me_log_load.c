/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/04, create
 */

# include "ut_mysql.h"
# include "me_market.h"
# include "me_balance.h"

static int do_load_orders(MYSQL *conn, const char *table)
{
    return 0;
}

static int do_load_balance(MYSQL *conn, const char *table)
{
    return 0;
}

static int do_load_oper_log(MYSQL *conn, const char *table)
{
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

