/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/01, create
 */

# ifndef _ME_OPERLOG_H_
# define _ME_OPERLOG_H_

# include "me_config.h"

int init_oper_log(void);
int fini_oper_log(void);

int append_oper_log(const char *method, json_t *params);

bool is_oper_log_block(void);

# endif

