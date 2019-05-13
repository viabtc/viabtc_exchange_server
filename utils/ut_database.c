/*
 * Description: 
 *     History: marat yusupov unlexx@gmail.com, 2019/02/25, create
 */

# include "ut_database.h"

struct db_query * db_malloc_query(sds mysql, sds pg){
    struct db_query  *query = malloc(sizeof(struct db_query));
    // need free after use
    query->mysql = mysql;
    query->postgresql = pg;
    return query;
}

bool is_need_connect_pg(struct db_connection * conn){
    return (conn->select_from == DB_READ_FROM_POSTGRESQL
           || conn->insert_into == DB_INSERT_INTO_POSTGRESQL
           || conn->insert_into == DB_INSERT_INTO_ALL);
}

bool is_need_connect_my(struct db_connection * conn){
    return (conn->select_from == DB_READ_FROM_MYSQL
            || conn->insert_into == DB_INSERT_INTO_MYSQL
            || conn->insert_into == DB_INSERT_INTO_ALL);
}

bool is_need_insert_my(struct db_connection * conn){
    return ( conn->insert_into == DB_INSERT_INTO_MYSQL
            || conn->insert_into == DB_INSERT_INTO_ALL);
}

bool is_need_insert_pg(struct db_connection * conn){
    return ( conn->insert_into == DB_INSERT_INTO_POSTGRESQL
             || conn->insert_into == DB_INSERT_INTO_ALL);
}

bool is_select_from_mysql(struct db_connection * conn){
    return (conn->select_from == DB_READ_FROM_MYSQL);
}

bool is_need_mysql(database_cfg *cfg){
    if ( strcmp(cfg->write_db_type, "all") == 0) {
        return true;
    } else if ( strcmp(cfg->write_db_type, "mysql") == 0) {
        return true;
    }
    if ( strcmp(cfg->read_db_type, "mysql") == 0) {
        return true;
    }
    return false;
}

bool is_need_pgsql(database_cfg *cfg){
    if ( strcmp(cfg->write_db_type, "all") == 0) {
        return true;
    } else if ( strcmp(cfg->write_db_type, "postgresql") == 0) {
        return true;
    }
    if ( strcmp(cfg->read_db_type, "postgresql") == 0) {
        return true;
    }
    return false;
}


struct db_connection *db_connect(database_cfg *cfg){
    struct db_connection *conn = malloc(sizeof(struct db_connection));
    // cfg->read_db_type = "mysql"  || "postgresql"
    // cfg->write_db_type = "all" || "mysql"  || "postgresql"
    if ( strcmp(cfg->write_db_type, "all") == 0) {
        conn->insert_into = DB_INSERT_INTO_ALL;
    } else if ( strcmp(cfg->write_db_type, "mysql") == 0) {
        conn->insert_into = DB_INSERT_INTO_MYSQL;
    } else {
        conn->insert_into = DB_INSERT_INTO_POSTGRESQL;
    }

    if ( strcmp(cfg->read_db_type, "mysql") == 0) {
        conn->select_from = DB_READ_FROM_MYSQL;
    } else {
        conn->select_from = DB_READ_FROM_POSTGRESQL;
    }
    conn->mysql = NULL;
    conn->postgresql = NULL;

    if (is_need_connect_my(conn)) {
        conn->mysql = mysql_connect(cfg);
    }

    if (is_need_connect_pg(conn)) {
        /* Make a connection to the database */
        conn->postgresql = PQconnectdb(cfg->conninfo);
        /* Check to see that the backend connection was successfully made */
        if (PQstatus(conn->postgresql) != CONNECTION_OK) {
            log_trace("Connection to database failed: %s", PQerrorMessage(conn->postgresql));
            PQfinish(conn->postgresql);
            return NULL;
        }
    }

    return conn;
}

void db_connection_free(struct db_connection * conn){
    if (conn->mysql != NULL) {
        mysql_close(conn->mysql);
    }
    if (conn->postgresql != NULL) {
        PQfinish(conn->postgresql);
    }
    /* close the connection to the database and cleanup */
    free(conn);
}