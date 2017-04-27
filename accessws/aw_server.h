/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/21, create
 */

# ifndef _AW_SERVER_H_
# define _AW_SERVER_H_

# include "aw_config.h"

struct clt_info {
    bool        auth;
    uint32_t    user_id;
    mpd_t       *taker_fee;
    mpd_t       *maker_fee;
};

int init_server(int worker_id);

# endif

