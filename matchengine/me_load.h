/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/04, create
 */

# ifndef _ME_LOAD_H_
# define _ME_LOAD_H_

# include <stdint.h>
# include "ut_database.h"

int load_orders(MYSQL *conn, const char *table);
int load_orders_from_postgresql(PGconn *conn, const char *table);
int load_balance(MYSQL *conn, const char *table);
int load_balance_from_postgresql(PGconn *conn, const char *table);

int load_operlog(MYSQL *conn, const char *table, uint64_t *start_id);
int load_operlog_from_postgresql(PGconn *conn, uint64_t *start_id);

# endif

