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

int send_error(nw_ses *ses, uint64_t id, int code, const char *message);
int send_error_invalid_argument(nw_ses *ses, uint64_t id);
int send_error_internal_error(nw_ses *ses, uint64_t id);
int send_error_service_unavailable(nw_ses *ses, uint64_t id);
int send_error_method_notfound(nw_ses *ses, uint64_t id);
int send_error_service_timeout(nw_ses *ses, uint64_t id);
int send_result(nw_ses *ses, uint64_t id, json_t *result);
int send_success(nw_ses *ses, uint64_t id);
int send_notify(nw_ses *ses, const char *method, json_t *params);

# endif

