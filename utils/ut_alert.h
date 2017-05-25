/*
 * Description: 
 *     History: yang@haipo.me, 2016/04/21, create
 */

# ifndef _UT_ALERT_H_
# define _UT_ALERT_H_

# include <stdarg.h>
# include <netinet/in.h>

typedef struct alert_cfg {
    char *host;
    struct sockaddr_in addr;
} alert_cfg;

int alert_init(alert_cfg *cfg);
int alert_msg(const char *fmt, ...);
int alert_vmsg(const char *fmt, va_list ap);

# endif

