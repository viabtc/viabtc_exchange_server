/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/04, create
 */

# include "me_config.h"
# include "me_persist.h"
# include "me_operlog.h"
# include "me_market.h"
# include "me_load.h"
# include "me_dump.h"

static time_t last_slice_time;
static nw_timer timer;

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

static int get_last_slice(struct db_connection *conn, time_t *timestamp, uint64_t *last_oper_id, uint64_t *last_order_id, uint64_t *last_deals_id)
{
    if (is_select_from_mysql(conn)) {
        sds sql = sdsempty();
        sql = sdscatprintf(sql,
                           "SELECT `time`, `end_oper_id`, `end_order_id`, `end_deals_id` from `slice_history` ORDER BY `id` DESC LIMIT 1");
        log_trace("exec mysql sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            log_stderr("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return -__LINE__;
        }
        sdsfree(sql);

        MYSQL_RES *result = mysql_store_result(conn->mysql);
        size_t num_rows = mysql_num_rows(result);
        if (num_rows != 1) {
            mysql_free_result(result);
            return 0;
        }

        MYSQL_ROW row = mysql_fetch_row(result);
        *timestamp = strtol(row[0], NULL, 0);
        *last_oper_id = strtoull(row[1], NULL, 0);
        *last_order_id = strtoull(row[2], NULL, 0);
        *last_deals_id = strtoull(row[3], NULL, 0);
        mysql_free_result(result);
    } else {
        // SELECT FROM postgre
        sds sql = sdsempty();
        sql = sdscatprintf(sql, "SELECT extract(epoch from time), end_oper_id, end_order_id, end_deals_id FROM \"slice_history\" ORDER BY id DESC LIMIT 1;");
        log_trace("exec postgresql sql: %s", sql);
        PGresult* res = PQexec(conn->postgresql, sql);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
            PQclear(res);
            sdsfree(sql);
            return -__LINE__;
        }
        sdsfree(sql);
        size_t num_rows = PQntuples(res);
        if (num_rows != 1) {
            PQclear(res);
            return 0;
        }
        const char* time_str = PQgetvalue(res, 0, 0);
        const char* end_oper_id_str = PQgetvalue(res, 0, 1);
        const char* end_order_id_str = PQgetvalue(res, 0, 2);
        const char* end_deals_id_str = PQgetvalue(res, 0, 3);
        *timestamp = strtol(time_str, NULL, 0);
        *last_oper_id = strtoull(end_oper_id_str, NULL, 0);
        *last_order_id = strtoull(end_order_id_str, NULL, 0);
        *last_deals_id = strtoull(end_deals_id_str, NULL, 0);
        PQclear(res);
    }

    return 0;
}

static int load_slice_from_db(struct db_connection *conn, time_t timestamp)
{
    sds table = sdsempty();

    table = sdscatprintf(table, "slice_order_%ld", timestamp);
    log_stderr("load orders from: %s", table);
    int ret =-1;
    if (is_select_from_mysql(conn)) {
        ret = load_orders(conn->mysql, table);
    } else {
        ret = load_orders_from_postgresql(conn->postgresql, table);
    }
    if (ret < 0) {
        log_error("load_orders from %s fail: %d", table, ret);
        log_stderr("load_orders from %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    sdsclear(table);
    table = sdscatprintf(table, "slice_balance_%ld", timestamp);
    log_stderr("load balance from: %s", table);
    if (is_select_from_mysql(conn)) {
        ret = load_balance(conn->mysql, table);
    } else {
        ret = load_balance_from_postgresql(conn->postgresql, table);
    }
    if (ret < 0) {
        log_error("load_balance from %s fail: %d", table, ret);
        log_stderr("load_balance from %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    sdsfree(table);
    return 0;
}

static int load_operlog_from_db(struct db_connection *conn, time_t date, uint64_t *start_id)
{
    sds table = NULL;
    table = get_operlog_table_name(date);
    log_stderr("load oper log from: %s", table);
    int ret =-1;
    if (is_select_from_mysql(conn)) {
        if (!is_table_exists(conn->mysql, table)) {
            log_error("table %s not exist", table);
            log_stderr("table %s not exist", table);
            sdsfree(table);
            return 0;
        }
        ret = load_operlog(conn->mysql, table, start_id);
    } else {
        ret = load_operlog_from_postgresql(conn->postgresql, start_id);
    }

    if (ret < 0) {
        log_error("load_operlog from %s fail: %d", table, ret);
        log_stderr("load_operlog from %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }

    sdsfree(table);
    return 0;
}

int init_from_db(void)
{
    struct db_connection *conn = db_connect(&settings.db_log);

    time_t now = time(NULL);
    uint64_t last_oper_id  = 0;
    uint64_t last_order_id = 0;
    uint64_t last_deals_id = 0;
    int ret = get_last_slice(conn, &last_slice_time, &last_oper_id, &last_order_id, &last_deals_id);
    if (ret < 0) {
        goto cleanup;
    }

    log_info("last_slice_time: %ld, last_oper_id: %"PRIu64", last_order_id: %"PRIu64", last_deals_id: %"PRIu64,
            last_slice_time, last_oper_id, last_order_id, last_deals_id);
    log_stderr("last_slice_time: %ld, last_oper_id: %"PRIu64", last_order_id: %"PRIu64", last_deals_id: %"PRIu64,
            last_slice_time, last_oper_id, last_order_id, last_deals_id);

    order_id_start = last_order_id;
    deals_id_start = last_deals_id;

    if (last_slice_time == 0) {
        ret = load_operlog_from_db(conn, now, &last_oper_id);
        if (ret < 0)
            goto cleanup;
    } else {
        ret = load_slice_from_db(conn, last_slice_time);
        if (ret < 0) {
            goto cleanup;
        }

        time_t begin = last_slice_time;
        time_t end = get_today_start() + 86400;
        while (begin < end) {
            ret = load_operlog_from_db(conn, begin, &last_oper_id);
            if (ret < 0) {
                goto cleanup;
            }
            begin += 86400;
        }
    }

    operlog_id_start = last_oper_id;

    db_connection_free(conn);
    log_stderr("load success");

    return 0;

cleanup:
    db_connection_free(conn);
    return ret;
}

static int dump_order_to_db(struct db_connection *conn, time_t end)
{
    sds table = sdsempty();
    table = sdscatprintf(table, "slice_order_%ld", end);
    log_info("dump order to: %s", table);
    int ret = dump_orders(conn, table);
    if (ret < 0) {
        log_error("dump_orders to %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }
    sdsfree(table);

    return 0;
}

static int dump_balance_to_db(struct db_connection *conn, time_t end)
{
    sds table = sdsempty();
    table = sdscatprintf(table, "slice_balance_%ld", end);
    log_info("dump balance to: %s", table);
    int ret = dump_balance(conn, table);
    if (ret < 0) {
        log_error("dump_balance to %s fail: %d", table, ret);
        sdsfree(table);
        return -__LINE__;
    }
    sdsfree(table);

    return 0;
}

int update_slice_history(struct db_connection *conn, time_t end_time)
{
    sds sql = sdsempty();
    log_info("update slice history to: %ld", end_time);
    if (is_need_insert_my(conn)) {
        sql = sdscatprintf(sql, "INSERT INTO `slice_history` (`id`, `time`, `end_oper_id`, `end_order_id`, `end_deals_id`) VALUES (NULL, %ld, %"PRIu64", %"PRIu64", %"PRIu64")",
                           end_time, operlog_id_start, order_id_start, deals_id_start);
        log_trace("exec mysql sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret < 0) {
            log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return -__LINE__;
        }
        sdsclear(sql);
    }
    if (is_need_insert_pg(conn)) {
        sql = sdscatprintf(sql, "INSERT INTO \"slice_history\" (time, end_oper_id, end_order_id, end_deals_id) "
                                "VALUES (to_timestamp(%ld), %"PRIu64", %"PRIu64", %"PRIu64")",
                           end_time, operlog_id_start, order_id_start, deals_id_start);
        log_trace("exec postgresql sql: %s", sql);
        PGresult* res = PQexec(conn->postgresql, sql);
        if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
            log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
            PQclear(res);
            sdsfree(sql);
            return -__LINE__;
        }
        PQclear(res);
    }

    sdsfree(sql);

    return 0;
}

int dump_to_db(time_t timestamp)
{
    struct db_connection *conn = db_connect(&settings.db_log);
    log_info("start dump slice, timestamp: %ld", timestamp);

    int ret;
    ret = dump_order_to_db(conn, timestamp);
    if (ret < 0) {
        goto cleanup;
    }

    ret = dump_balance_to_db(conn, timestamp);
    if (ret < 0) {
        goto cleanup;
    }

    ret = update_slice_history(conn, timestamp);
    if (ret < 0) {
        goto cleanup;
    }

    log_info("dump success");
    db_connection_free(conn);
    return 0;

cleanup:
    log_info("dump fail");
    db_connection_free(conn);
    return ret;
}

static int slice_count(struct db_connection *conn, time_t timestamp)
{
    sds sql = sdsempty();
    if (is_select_from_mysql(conn)) {
        sql = sdscatprintf(sql, "SELECT COUNT(*) FROM `slice_history` WHERE `time` >= %ld",
                           timestamp - settings.slice_keeptime);
        log_trace("exec mysql sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return -__LINE__;
        }
        sdsfree(sql);

        MYSQL_RES *result = mysql_store_result(conn->mysql);
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

    sql = sdscatprintf(sql, "SELECT COUNT(*) FROM \"slice_history\" WHERE time >= to_timestamp(%ld) ",
                       timestamp - settings.slice_keeptime);
    log_trace("exec postgresql sql: %s", sql);
    PGresult* res = PQexec(conn->postgresql, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        sdsfree(sql);
        return -__LINE__;
    }
    sdsfree(sql);
    size_t num_rows = PQntuples(res);
    if (num_rows != 1) {
        PQclear(res);
        return -__LINE__;
    }

    char* row = PQgetvalue(res, 0, 0);
    int count = atoi(row);
    PQclear(res);

    return count;
}

static int delete_slice(MYSQL *conn, uint64_t id, time_t timestamp)
{
    log_info("delete slice id: %"PRIu64", time: %ld start", id, timestamp);

    int ret;
    sds sql = sdsempty();
    sql = sdscatprintf(sql, "DROP TABLE IF EXISTS `slice_order_%ld`", timestamp);
    log_trace("exec sql: %s", sql);
    ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret != 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        return -__LINE__;
    }
    sdsclear(sql);

    sql = sdscatprintf(sql, "DROP TABLE IF EXISTS `slice_balance_%ld`", timestamp);
    log_trace("exec sql: %s", sql);
    ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret != 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        return -__LINE__;
    }
    sdsclear(sql);

    sql = sdscatprintf(sql, "DELETE FROM `slice_history` WHERE `id` = %"PRIu64"", id);
    log_trace("exec sql: %s", sql);
    ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret != 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        return -__LINE__;
    }
    sdsfree(sql);

    log_info("delete slice id: %"PRIu64", time: %ld success", id, timestamp);

    return 0;
}

static int delete_slice_list(MYSQL *conn, time_t timestamp){
    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT `id`, `time` FROM `slice_history` WHERE `time` < %ld", timestamp - settings.slice_keeptime);
    log_trace("exec mysql sql: %s", sql);
    int ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret != 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        sdsfree(sql);
        return  -__LINE__;
    }
    sdsfree(sql);

    MYSQL_RES *result = mysql_store_result(conn);
    size_t num_rows = mysql_num_rows(result);
    for (size_t i = 0; i < num_rows; ++i) {
        MYSQL_ROW row = mysql_fetch_row(result);
        uint64_t id = strtoull(row[0], NULL, 0);
        time_t slice_time = strtol(row[1], NULL, 0);
        ret = delete_slice(conn, id, slice_time);
        if (ret < 0) {
            mysql_free_result(result);
            return  -__LINE__;
        }

    }
    mysql_free_result(result);
    return  0;
}

static int delete_slice_postgresql(PGconn *conn, time_t timestamp){
    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT id, extract(epoch from time) FROM \"slice_history\" WHERE time < to_timestamp(%ld)", timestamp - settings.slice_keeptime);
    log_trace("exec postgresql sql: %s", sql);
    PGresult* result = PQexec(conn, sql);
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(result));
        PQclear(result);
        sdsfree(sql);
        return -__LINE__;
    }
    sdsclear(sql);

    size_t num_rows = PQntuples(result);
    for (size_t i = 0; i < num_rows; ++i) {
        char* id_str = PQgetvalue(result, i, 0);
        uint64_t id = strtoull(id_str, NULL, 0);
        char* slice_time_str = PQgetvalue(result, i, 1);
        time_t slice_time = strtol(slice_time_str, NULL, 0);
        sql = sdscatprintf(sql, "DROP TABLE IF EXISTS \"slice_order_%ld\";", slice_time);
        sql = sdscatprintf(sql, "DROP TABLE IF EXISTS \"slice_balance_%ld\";", slice_time);
        sql = sdscatprintf(sql, "DELETE FROM \"slice_history\" WHERE id = %"PRIu64";", id);
        log_trace("exec postgresql sql: %s", sql);

        PGresult* res = PQexec(conn, sql);
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            log_error("exec postgresql: %s fail: %s", sql, PQresultErrorMessage(res));
            PQclear(res);
            sdsfree(sql);
            return -__LINE__;
        }
        sdsclear(sql);
        PQclear(res);
    }
    PQclear(result);
    sdsfree(sql);

    return  0;
}

int clear_slice(time_t timestamp)
{
    struct db_connection *conn = db_connect(&settings.db_log);

    int ret = slice_count(conn, timestamp);
    if (ret < 0) {
        goto cleanup;
    }
    if (ret == 0) {
        log_error("0 slice in last %d seconds", settings.slice_keeptime);
        goto cleanup;
    }
    if (is_need_insert_my(conn)) {
        ret = delete_slice_list(conn->mysql, timestamp);
        if (ret < 0) {
            goto cleanup;
        }
    }
    if (is_need_insert_pg(conn)) {
        ret = delete_slice_postgresql(conn->postgresql, timestamp);
        if (ret < 0) {
            goto cleanup;
        }
    }

    db_connection_free(conn);
    return 0;

cleanup:
    db_connection_free(conn);
    return ret;
}

int make_slice(time_t timestamp)
{
    int pid = fork();
    if (pid < 0) {
        log_fatal("fork fail: %d", pid);
        return -__LINE__;
    } else if (pid > 0) {
        return 0;
    }

    int ret;
    ret = dump_to_db(timestamp);
    if (ret < 0) {
        log_fatal("dump_to_db fail: %d", ret);
    }

    ret = clear_slice(timestamp);
    if (ret < 0) {
        log_fatal("clear_slice fail: %d", ret);
    }

    exit(0);
    return 0;
}

static void on_timer(nw_timer *timer, void *privdata)
{
    time_t now = time(NULL);
    if ((now - last_slice_time) >= settings.slice_interval && (now % settings.slice_interval) <= 5) {
        make_slice(now);
        last_slice_time = now;
    }
}

int init_persist(void)
{
    nw_timer_set(&timer, 1.0, true, on_timer, NULL);
    nw_timer_start(&timer);

    return 0;
}

