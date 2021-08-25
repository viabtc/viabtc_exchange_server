/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/01, create
 */

# include "me_config.h"
# include "me_operlog.h"

uint64_t operlog_id_start;

static MYSQL *mysql_conn;
static nw_job *job;
static list_t *list;
static nw_timer timer;

struct operlog {
    uint64_t id;
    double create_time;
    char *detail;
};

static void *on_job_init(void)
{
    struct db_connection *conn = db_connect(&settings.db_log);
    return conn;
}

static void on_job(nw_job_entry *entry, void *privdata) {
    struct db_connection *db = privdata;
    struct db_query *query = entry->request;
    if (is_need_insert_my(db)) {
        MYSQL *conn = db->mysql;
        sds sql = query->mysql;
        log_trace("exec mysql sql: %s", sql);
        while (true) {
            int ret = mysql_real_query(conn, sql, sdslen(sql));
            if (ret != 0 && mysql_errno(conn) != 1062) {
                log_fatal("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
                usleep(1000 * 1000);
                continue;
            }
            break;
        }
    }
    if (is_need_insert_pg(db)) {
        // TODO add retry? if fail postgresql
        sds sql = query->postgresql;
        log_trace("exec postgresql sql: %s", sql);
        PGresult *res;
        res = PQexec(db->postgresql, sql);
        if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
            log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        }

        PQclear(res);
    }
}

static void on_job_cleanup(nw_job_entry *entry)
{
    struct db_query *query = entry->request;
    sdsfree(query->mysql);
    sdsfree(query->postgresql);
    free(entry->request);
}

static void on_job_release(void *privdata)
{
    struct db_connection *db = privdata;
    db_connection_free(db);
}

static void on_list_free(void *value)
{
    struct operlog *log = value;
    free(log->detail);
    free(log);
}

sds get_current_operlog_table_name() {
    time_t now = time(NULL);
    return get_operlog_table_name(now);
}

sds get_operlog_table_name(time_t now) {
    sds result = sdsempty();
    struct tm *today = localtime(&now);
    int table_name_len = sizeof("operlog_20190222");
    char buffer[table_name_len];
    strftime (buffer, table_name_len, "operlog_%Y%m%d", today);
    return sdscat(result, buffer);
}

sds get_partition_condition() {
    // "('2019-02-22') TO ('2019-02-22')"
    sds result = sdsempty();
    int date_len = sizeof("2019-02-22");
    char today_str[date_len];
    char nextday_str[date_len];

    time_t now = time(NULL);
    struct tm *tmp_tm = localtime(&now);
    strftime(today_str, date_len, "%Y-%m-%d", tmp_tm);
    tmp_tm->tm_mday = tmp_tm->tm_mday + 1;
    tmp_tm->tm_isdst = -1;        // don't know if DST is in effect, please determine this for me
    time_t next = mktime(tmp_tm);
    tmp_tm = localtime(&next);
    strftime(nextday_str, date_len, "%F", tmp_tm);
    result = sdscatprintf(result, "('%s') TO ('%s')", today_str, nextday_str);
    return result;
}

static void flush_log(void)
{
    static sds table_last;
    if (table_last == NULL) {
        table_last = sdsempty();
    }
    sds table = NULL;
    table = get_current_operlog_table_name();

    // CREATE TABLE operlog_20190221 PARTITION of operlog FOR VALUES
    //    FROM ('2019-02-21') TO ('2019-02-22');
    if (sdscmp(table_last, table) != 0) {
        sds create_table_mysql = sdsempty();
        if (use_mysql)
            create_table_mysql = sdscatprintf(create_table_mysql, "CREATE TABLE IF NOT EXISTS `%s` like `operlog_example`", table);

        sds create_table_pq = sdsempty();
        if (use_postgresql) {
            sds cond_str = NULL;
            cond_str = get_partition_condition();
            create_table_pq = sdscatprintf(create_table_pq, "CREATE TABLE IF NOT EXISTS \"%s\" PARTITION "
                                                            "OF operlog FOR VALUES FROM %s ", table, cond_str);
            sdsfree(cond_str);
        }
        struct db_query  *query = db_malloc_query(create_table_mysql, create_table_pq);
        nw_job_add(job, 0, query);
        table_last = sdscpy(table_last, table);
    }

    sds mysql_query = sdsempty();
    if (use_mysql)
        mysql_query = sdscatprintf(mysql_query, "INSERT INTO `%s` (`id`, `time`, `detail`) VALUES ", table);
    sdsfree(table);
    sds pg_query = sdsempty();
    if (use_postgresql)
        pg_query = sdscat(pg_query, "INSERT INTO operlog (id, query, time, query_date) VALUES ");
    size_t count=0;
    char buf[10240];
    list_node *node;
    list_iter *iter = list_get_iterator(list, LIST_START_HEAD);
    while ((node = list_next(iter)) != NULL) {
        struct operlog *log = node->value;
        if (use_mysql) {
            size_t detail_len = strlen(log->detail);
            mysql_real_escape_string(mysql_conn, buf, log->detail, detail_len);
            mysql_query = sdscatprintf(mysql_query, "(%"PRIu64", %f, '%s')", log->id, log->create_time, buf);
        }
        if (use_postgresql)
            pg_query = sdscatprintf(pg_query, "(%"PRIu64"::bigint,'%s'::jsonb, to_timestamp(%f), to_timestamp(%f))",
                log->id, log->detail,  log->create_time,  log->create_time);
        if (list_len(list) > 1) {
            if (use_mysql)
                mysql_query = sdscatprintf(mysql_query, ", ");
            if (use_postgresql)
                pg_query = sdscatprintf(pg_query, ", ");
        }
        list_del(list, node);
        count++;
        // TODO query need break into chunks  matchengine/me_dump.c:63
    }
    list_release_iterator(iter);
    struct db_query  *query = db_malloc_query(mysql_query, pg_query);
    nw_job_add(job, 0, query);

    log_debug("flush oper log count: %zu", count);
}

static void on_timer(nw_timer *t, void *privdata)
{
    if (list->len > 0) {
        flush_log();
    }
}

int init_operlog(void)
{
    use_mysql = false;
    use_postgresql = false;
    if (is_need_mysql(&settings.db_log)){
        use_mysql = true;
    }
    if (is_need_pgsql(&settings.db_log)){
        use_postgresql = true;
    }
    if (use_mysql) {
        mysql_conn = mysql_init(NULL);
        if (mysql_conn == NULL)
            return -__LINE__;
        if (mysql_options(mysql_conn, MYSQL_SET_CHARSET_NAME, settings.db_log.charset) != 0)
            return -__LINE__;
    }

    nw_job_type type;
    memset(&type, 0, sizeof(type));
    type.on_init    = on_job_init;
    type.on_job     = on_job;
    type.on_cleanup = on_job_cleanup;
    type.on_release = on_job_release;

    job = nw_job_create(&type, 1);
    if (job == NULL)
        return -__LINE__;

    list_type lt;
    memset(&lt, 0, sizeof(lt));
    lt.free = on_list_free;
    list = list_create(&lt);
    if (list == NULL)
        return -__LINE__;

    nw_timer_set(&timer, 0.1, true, on_timer, NULL);
    nw_timer_start(&timer);

    return 0;
}

int fini_operlog(void)
{
    on_timer(NULL, NULL);

    usleep(100 * 1000);
    nw_job_release(job);
    if (use_mysql)
        mysql_close(mysql_conn);

    return 0;
}

int append_operlog(const char *method, json_t *params)
{
    json_t *detail = json_object();
    json_object_set_new(detail, "method", json_string(method));
    json_object_set(detail, "params", params);
    struct operlog *log = malloc(sizeof(struct operlog));
    log->id = ++operlog_id_start;
    log->create_time = current_timestamp();
    log->detail = json_dumps(detail, JSON_SORT_KEYS);
    json_decref(detail);
    list_add_node_tail(list, log);
    log_debug("add log: %s", log->detail);

    return 0;
}

bool is_operlog_block(void)
{
    if (job->request_count >= MAX_PENDING_OPERLOG)
        return true;
    return false;
}

sds operlog_status(sds reply)
{
    reply = sdscatprintf(reply, "operlog last ID: %"PRIu64"\n", operlog_id_start);
    reply = sdscatprintf(reply, "operlog pending: %d\n", job->request_count);
    return reply;
}

