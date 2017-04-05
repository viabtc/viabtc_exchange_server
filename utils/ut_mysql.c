/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/05, create
 */

# include "ut_log.h"
# include "ut_mysql.h"

MYSQL *mysql_connect(mysql_cfg *db)
{
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL)
        return NULL;

    my_bool reconnect = 1;
    if (mysql_options(conn, MYSQL_OPT_RECONNECT, &reconnect) != 0) {
        mysql_close(conn);
        return NULL;
    }
    if (mysql_options(conn, MYSQL_SET_CHARSET_NAME, db->charset) != 0) {
        mysql_close(conn);
        return NULL;
    }
    if (mysql_real_connect(conn, db->host, db->user, db->pass, db->name, db->port, NULL, 0) == NULL) {
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

bool is_table_exists(MYSQL *conn, const char *table)
{
    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SHOW TABLES LIKE '%s'", table);
    log_trace("exec sql: %s", sql);
    int ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret < 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        sdsfree(sql);
        return false;
    }
    sdsfree(sql);

    MYSQL_RES *result = mysql_store_result(conn);
    size_t num_rows = mysql_num_rows(result);
    mysql_free_result(result);
    if (num_rows == 1)
        return true;

    return false;
}

