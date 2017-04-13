/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/04, create
 */

# ifndef _ME_PERSIST_H_
# define _ME_PERSIST_H_

# include <time.h>

int init_persist(void);

int init_from_db(void);
int dump_to_db(time_t timestamp);
int make_slice(time_t timestamp);
int clear_slice(time_t timestamp);

# endif

