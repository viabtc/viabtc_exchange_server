/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/01, create
 */

# include "me_log.h"
# include "nw_job.h"
# include "ut_list.h"

# include <mysql/mysql.h>
# include <mysql/errmsg.h>

static MYSQL *mysql_conn;
static nw_job *job;
static list_t *log_list;
static nw_timer timer;

struct oper_log {
    time_t create_time;
    char *detail;
};

static void *on_job_init(void)
{
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL)
        return NULL;

    my_bool reconnect = 1;
    if (mysql_options(conn, MYSQL_OPT_RECONNECT, &reconnect) != 0)
        return NULL;
    if (mysql_options(conn, MYSQL_SET_CHARSET_NAME, settings.db.charset) != 0)
        return NULL;
    if (mysql_real_connect(conn, settings.db.host, settings.db.user, settings.db.pass,
                settings.db.name, settings.db.port, NULL, 0) == NULL)
        return NULL;

    return conn;
}

static void on_job(nw_job_entry *entry, void *privdata)
{
    MYSQL *conn = privdata;
    const char *sql = entry->request;
    log_trace("exec sql: %s", sql);
    while (true) {
        int ret = mysql_real_query(conn, sql, (unsigned long)strlen(sql));
        if (ret != 0) {
            unsigned errcode = mysql_errno(conn);
            log_error("exec sql: %s fail: %u %s", sql, errcode, (char *)mysql_error(conn));
            usleep(1000 * 1000);
            continue;
        }

        MYSQL_RES *result = mysql_store_result(conn);
        if (result != NULL)
            mysql_free_result(result);
        break;
    }
}

static void on_job_cleanup(nw_job_entry *entry)
{
    sdsfree(entry->request);
}

static void on_job_release(void *privdata)
{
    mysql_close(privdata);
}

static void on_list_free(void *value)
{
    struct oper_log *log = value;
    free(log->detail);
    free(log);
}

static void flush_log(void)
{
    static sds table_last;
    if (table_last == NULL) {
        table_last = sdsempty();
    }

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    sds table = sdsempty();
    table = sdscatprintf(table, "oper_log_%04d%02d%02d", 1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday);

    if (sdscmp(table_last, table) != 0) {
        sds create_table_sql = sdsempty();
        create_table_sql = sdscatprintf(create_table_sql, "CREATE TABLE IF NOT EXISTS `%s` like `oper_log_example`;", table);
        nw_job_add(job, 0, create_table_sql);
        sdsclear(table_last);
        table_last = sdscatsds(table_last, table);
    }

    sds sql = sdsempty();
    sql = sdscatprintf(sql, "INSERT INTO `%s` (`id`, `time`, `detail`) VALUES ", table);
    sdsfree(table);

    char buf[10240];
    list_node *node;
    list_iter *iter = list_get_iterator(log_list, LIST_START_HEAD);
    while ((node = list_next(iter)) != NULL) {
        struct oper_log *log = node->value;
        size_t detail_len = strlen(log->detail);
        mysql_real_escape_string(mysql_conn, buf, log->detail, detail_len);
        sql = sdscatprintf(sql, "(NULL, %ld, '%s')", log->create_time, buf);
        if (list_len(log_list) > 1) {
            sql = sdscatprintf(sql, ", ");
        }
        list_del(log_list, node);
    }
    list_release_iterator(iter);
    nw_job_add(job, 0, sql);
}

static void on_timer(nw_timer *timer, void *privdata)
{
    if (log_list->len > 0) {
        flush_log();
    }
}

int init_oper_log(void)
{
    mysql_conn = mysql_init(NULL);
    if (mysql_conn == NULL)
        return -__LINE__;
    if (mysql_options(mysql_conn, MYSQL_SET_CHARSET_NAME, settings.db.charset) != 0)
        return -__LINE__;

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
    log_list = list_create(&lt);
    if (log_list == NULL)
        return -__LINE__;

    nw_timer_set(&timer, 0.1, true, on_timer, NULL);
    nw_timer_start(&timer);

    return 0;
}

int append_oper_log(const char *method, json_t *params)
{
    json_t *detail = json_object();
    json_object_set_new(detail, "method", json_string(method));
    json_object_set(detail, "params", params);
    struct oper_log *log = malloc(sizeof(struct oper_log));
    log->create_time = time(NULL);
    log->detail = json_dumps(detail, 0);
    json_decref(detail);
    list_add_node_tail(log_list, log);
    log_debug("add log: %s", log->detail);

    return 0;
}

