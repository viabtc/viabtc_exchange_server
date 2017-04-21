/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/21, create
 */

# include "ah_config.h"
# include "ah_server.h"

static http_svr *svr;

static int on_http_request(nw_ses *ses, http_request_t *request)
{
    sds response = sdsnew("Hello world!");
    int ret = send_http_response_simple(ses, 200, response);
    sdsfree(response);
    return ret;
}

int init_server(void)
{
    svr = http_svr_create(&settings.svr, on_http_request);
    if (svr == NULL)
        return -__LINE__;
    if (http_svr_start(svr) < 0)
        return -__LINE__;

    return 0;
}

