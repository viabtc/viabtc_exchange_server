/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/05, create
 */

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

