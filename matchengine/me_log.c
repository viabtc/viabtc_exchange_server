/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/01, create
 */

# include "me_log.h"
# include "nw_job.h"
# include "ut_list.h"

# include <mysql/mysql.h>
# include <mysql/errmsg.h>

static nw_job *job;
static list_t *log_list;

static void *on_job_init(void)
{
    return NULL;
}

static void on_job(nw_job_entry *entry, void *privdata)
{
}

static void on_job_cleanup(nw_job_entry *entry)
{
}

static void on_job_release(void *privdata)
{
}

static void on_list_free(void *value)
{
}

int init_oper_log(void)
{
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

    return 0;
}

int append_oper_log(const char *type, json_t *params)
{
    return 0;
}
