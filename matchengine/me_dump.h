/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/04, create
 */

# ifndef _ME_DUMP_H_
# define _ME_DUMP_H_

# include "ut_mysql.h"

int dump_orders(struct db_connection *conn, const char *table);
int dump_balance(struct db_connection *conn, const char *table);

# endif

