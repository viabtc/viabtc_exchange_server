/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/01, create
 */

# ifndef _ME_OPERLOG_H_
# define _ME_OPERLOG_H_

# include "me_config.h"

int init_operlog(void);
int fini_operlog(void);

int append_operlog(const char *method, json_t *params);

bool is_operlog_block(void);

# endif

