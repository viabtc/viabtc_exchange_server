/*
 * Description: 
 *     History: yang@haipo.me, 2016/04/21, create
 */

# include <stdio.h>
# include <string.h>
# include <sys/socket.h>

# include "ut_sds.h"
# include "ut_log.h"
# include "ut_alert.h"

static const char *host;
static struct sockaddr_in addr;
static int sockfd;
static int init_flag;
static const char *magic_head = "373d26968a5a2b698045";
static const int msg_max_size = 10240;

int alert_init(alert_cfg *cfg)
{
    if (init_flag)
        return 0;
    host = strdup(cfg->host);
    memcpy(&addr, &cfg->addr, sizeof(addr));
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        return -1;
    dlog_on_fatal = alert_msg;
    init_flag = 1;
    return 0;
}

int alert_vmsg(const char *fmt, va_list ap)
{
    if (!init_flag)
        return -__LINE__;
    sds msg = sdsempty();
    msg = sdscat(msg, magic_head);
    msg = sdscatprintf(msg, "%s: ", host);
    msg = sdscatvprintf(msg, fmt, ap);
    msg = sdscatprintf(msg, "\n");
    size_t msg_len = sdslen(msg);
    if (sdslen(msg) > msg_max_size)
        msg_len = msg_max_size;
    if (sendto(sockfd, msg, msg_len, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        sdsfree(msg);
        return -__LINE__;
    }
    sdsfree(msg);
    return 0;
}

int alert_msg(const char *fmt, ...)
{
    if (!init_flag)
        return -__LINE__;
    va_list ap;
    va_start(ap, fmt);
    int ret = alert_vmsg(fmt, ap);
    va_end(ap);
    return ret;
}

