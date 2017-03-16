/*
 * Description: user balance manage
 *     History: yang@haipo.me, 2017/03/15, create
 */

# ifndef _ME_BALANCE_H_
# define _ME_BALANCE_H_

# define BALANCE_TYPE_AVAILABLE 0
# define BALANCE_TYPE_FREEZE    1

int init_balance(void);
bool asset_exist(const char *asset);

# endif

