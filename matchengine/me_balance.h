/*
 * Description: user balance manage
 *     History: yang@haipo.me, 2017/03/15, create
 */

# ifndef _ME_BALANCE_H_
# define _ME_BALANCE_H_

# include "me_config.h"

# define BALANCE_TYPE_AVAILABLE 0
# define BALANCE_TYPE_FREEZE    1

int init_balance(void);
int asset_exist(const char *asset);

mpd_t *balance_get(uint32_t user_id, uint32_t type, const char *asset);
void   balance_del(uint32_t user_id, uint32_t type, const char *asset);
mpd_t *balance_set(uint32_t user_id, uint32_t type, const char *asset, mpd_t *amount);
mpd_t *balance_add(uint32_t user_id, uint32_t type, const char *asset, mpd_t *amount);
mpd_t *balance_sub(uint32_t user_id, uint32_t type, const char *asset, mpd_t *amount);
mpd_t *balance_freeze(uint32_t user_id, const char *asset, mpd_t *amount);
mpd_t *balance_unfreeze(uint32_t user_id, const char *asset, mpd_t *amount);

typedef void (*balance_traverse_callback)(uint32_t user_id, uint32_t type, const char *asset, const mpd_t *amount, void *privdata);
void balance_traverse(balance_traverse_callback callback, void *privdata);

# endif

