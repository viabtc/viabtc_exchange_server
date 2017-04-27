/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/21, create
 */

# ifndef _UT_HTTP_
# define _UT_HTTP_

# include "ut_dict.h"
# include "ut_sds.h"
# include "nw_ses.h"
# include "http_parser.h"

typedef struct http_request_t {
    uint16_t    version_major;
    uint16_t    version_minor;
    uint32_t    method;
    dict_t      *headers;
    sds         url;
    sds         body;
} http_request_t;

typedef struct http_response_t {
    uint32_t    status;
    dict_t      *headers;
    void        *content;
    size_t      content_size;
} http_response_t;

http_request_t *http_request_new(void);
int http_request_set_header(http_request_t *request, char *field, char *value);
const char *http_request_get_header(http_request_t *request, const char *field);
void http_request_release(http_request_t *request);

http_response_t *http_response_new(void);
int http_response_set_header(http_response_t *response, char *field, char *value);
const char *http_response_get_header(http_response_t *response, const char *field);
sds http_response_encode(http_response_t *response);
void http_response_release(http_response_t *response);

const char *http_get_remote_ip(nw_ses *ses, http_request_t *request);

# endif

