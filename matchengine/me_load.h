/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/04, create
 */

# ifndef _ME_LOAD_H_
# define _ME_LOAD_H_

# include "ut_mysql.h"

int load_orders(MYSQL *conn, const char *table);
int load_markets(MYSQL *conn, const char *table);
int load_balance(MYSQL *conn, const char *table);
int load_operlog(MYSQL *conn, const char *table);

# endif

