/*
 * Description: 
 *     History: marat yusupov unlexx@gmail.com, 2019/02/25, create
 */

# ifndef _UT_DATABASE_H_
# define _UT_DATABASE_H_

# include <libpq-fe.h>

# include "ut_config.h"
# include "ut_mysql.h"

enum {
    DB_READ_FROM_MYSQL        = 1,
    DB_READ_FROM_POSTGRESQL   = 2,
    DB_INSERT_INTO_POSTGRESQL = 3,
    DB_INSERT_INTO_MYSQL      = 4,
    DB_INSERT_INTO_ALL        = 5,
};

struct db_connection {
    MYSQL *mysql;
    PGconn *postgresql;
    uint32_t select_from;
    uint32_t insert_into;
};

struct db_query {
    sds mysql;
    sds postgresql;
};

struct db_query * db_malloc_query(sds mysql, sds pg);
struct db_connection *db_connect(database_cfg *cfg);
void db_connection_free(struct db_connection * conn);
bool is_need_connect_pg(struct db_connection * conn);
bool is_need_connect_my(struct db_connection * conn);
bool is_need_insert_my(struct db_connection * conn);
bool is_need_insert_pg(struct db_connection * conn);
bool is_select_from_mysql(struct db_connection * conn);
bool is_need_insert_pg(struct db_connection * conn);
bool is_need_mysql(database_cfg *cfg);
bool is_need_pgsql(database_cfg *cfg);

# endif

