/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/21, create
 */

# ifndef _UT_HTTP_SVR_H_
# define _UT_HTTP_SVR_H_

# include "ut_http.h"
# include "nw_buf.h"
# include "nw_svr.h"
# include "nw_timer.h"

typedef struct http_svr_cfg {
    uint32_t bind_count;
    nw_svr_bind *bind_arr;
    uint32_t max_pkg_size;
    uint32_t buf_limit;
    uint32_t read_mem;
    uint32_t write_mem;
    int keep_alive;
} http_svr_cfg;

typedef int (*http_request_callback)(nw_ses *ses, http_request_t *request);

typedef struct http_svr {
    nw_svr *raw_svr;
    nw_timer timer;
    nw_cache *privdata_cache;
    int keep_alive;
    http_parser_settings settings;
    http_request_callback on_request;
} http_svr;

http_svr *http_svr_create(http_svr_cfg *cfg, http_request_callback on_request);
int http_svr_start(http_svr *svr);
int http_svr_stop(http_svr *svr);
int send_http_response(nw_ses *ses, http_response_t *response);
int send_http_response_simple(nw_ses *ses, uint32_t status, void *content, size_t size);
http_svr *http_svr_from_ses(nw_ses *ses);
void http_svr_close_clt(http_svr *svr, nw_ses *ses);
void http_svr_release(http_svr *svr);

# endif

