/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/18, create
 */

# ifndef _ME_UPDATE_H_
# define _ME_UPDATE_H_

int init_update(void);
int update_user_balance(bool real, uint32_t user_id, uint32_t type,
        const char *asset, const char *business, uint64_t business_id, mpd_t *change);

# endif

