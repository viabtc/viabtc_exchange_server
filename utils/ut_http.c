/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/21, create
 */

# include <string.h>
# include <stdlib.h>
# include <time.h>

# include "ut_http.h"
# include "ut_misc.h"

static uint32_t dict_header_hash_func(const void *key)
{
    char *tmp = strdup(key);
    size_t len = strlen(tmp);
    for (size_t i = 0; i < len; ++i) {
        if (tmp[i] >= 'A' && tmp[i] <= 'Z')
            tmp[i] += 'a' - 'A';
    }
    uint32_t hash = dict_generic_hash_function(tmp, len);
    free(tmp);

    return hash;
}

static int dict_header_key_compare(const void *key1, const void *key2)
{
    return strcasecmp(key1, key2);
}

static void *dict_header_dup(const void *obj)
{
    return strdup(obj);
}

static void dict_header_free(void *obj)
{
    free(obj);
}

static dict_t *create_header_dict(void)
{
    dict_types dt;
    memset(&dt, 0, sizeof(dt));
    dt.hash_function = dict_header_hash_func;
    dt.key_compare = dict_header_key_compare;
    dt.key_dup = dict_header_dup;
    dt.key_destructor = dict_header_free;
    dt.val_dup = dict_header_dup;
    dt.val_destructor = dict_header_free;

    return dict_create(&dt, 64);
}

http_request_t *http_request_new(void)
{
    http_request_t * request = malloc(sizeof(http_request_t));
    if (request == NULL)
        return NULL;
    memset(request, 0, sizeof(http_request_t));
    request->headers = create_header_dict();
    if (request->headers == NULL) {
        free(request);
        return NULL;
    }

    return request;
}

int http_request_set_header(http_request_t *request, char *field, char *value)
{
    dict_entry *entry = dict_find(request->headers, field);
    if (entry) {
        char *new = realloc(entry->val, strlen(entry->val) + 2 + strlen(value) + 1);
        strcat(new, ", ");
        strcat(new, value);
        entry->val= new;
        return 0;
    }

    if (dict_add(request->headers, field, value) == NULL)
        return -1;
    return 0;
}

const char *http_request_get_header(http_request_t *request, const char *field)
{
    dict_entry *entry = dict_find(request->headers, field);
    if (entry)
        return entry->val;
    return NULL;
}

void http_request_release(http_request_t *request)
{
    if (request->headers)
        dict_release(request->headers);
    if (request->url)
        sdsfree(request->url);
    if (request->body)
        sdsfree(request->body);
    free(request);
}

http_response_t *http_response_new(void)
{
    http_response_t *response = malloc(sizeof(http_response_t));
    if (response == NULL)
        return NULL;
    memset(response, 0, sizeof(http_response_t));
    response->headers = create_header_dict();
    if (response->headers == NULL) {
        free(response);
        return NULL;
    }

    return response;
}

int http_response_set_header(http_response_t *response, char *field, char *value)
{
    dict_entry *entry = dict_find(response->headers, field);
    if (entry) {
        char *new = realloc(entry->val, strlen(entry->val) + 2 + strlen(value) + 1);
        strcat(new, ", ");
        strcat(new, value);
        entry->val = new;
        return 0;
    }

    if (dict_add(response->headers, field, value) == NULL)
        return -1;
    return 0;
}

const char *http_response_get_header(http_response_t *response, const char *field)
{
    dict_entry *entry = dict_find(response->headers, field);
    if (entry)
        return entry->val;
    return NULL;
}

const char *get_status_description(uint32_t status)
{
    switch (status) {
    case 100:
        return "Continue";
    case 101:
        return "Switching Protocols";
    case 102:
        return "Processing";
    case 200:
        return "OK";
    case 201:
        return "Created";
    case 202:
        return "Accepted";
    case 203:
        return "Non-Authoritative Information";
    case 204:
        return "No Content";
    case 205:
        return "Reset Content";
    case 206:
        return "Partial Content";
    case 207:
        return "Multi-Status";
    case 208:
        return "Already Reported";
    case 226:
        return "IM Used";
    case 300:
        return "Multiple Choices";
    case 301:
        return "Moved Permanently";
    case 302:
        return "Found";
    case 303:
        return "See Other";
    case 304:
        return "Not Modified";
    case 305:
        return "Use Proxy";
    case 307:
        return "Temporary Redirect";
    case 308:
        return "Permanent Redirect";
    case 400:
        return "Bad Request";
    case 401:
        return "Unauthorized";
    case 402:
        return "Payment Required";
    case 403:
        return "Forbidden";
    case 404:
        return "Not Found";
    case 405:
        return "Method Not Allowed";
    case 406:
        return "Not Acceptable";
    case 407:
        return "Proxy Authentication Required";
    case 408:
        return "Request Timeout";
    case 409:
        return "Conflict";
    case 410:
        return "Gone";
    case 411:
        return "Length Required";
    case 412:
        return "Precondition Failed";
    case 413:
        return "Payload Too Large";
    case 414:
        return "URI Too Long";
    case 415:
        return "Unsupported Media Type";
    case 416:
        return "Range Not Satisfiable";
    case 417:
        return "Expectation Failed";
    case 421:
        return "Misdirected Request";
    case 422:
        return "Unprocessable Entity";
    case 423:
        return "Locked";
    case 424:
        return "Failed Dependency";
    case 426:
        return "Upgrade Required";
    case 428:
        return "Precondition Required";
    case 429:
        return "Too Many Requests";
    case 431:
        return "Request Header Fields Too Large";
    case 451:
        return "Unavailable For Legal Reasons";
    case 500:
        return "Internal Server Error";
    case 501:
        return "Not Implemented";
    case 502:
        return "Bad Gateway";
    case 503:
        return "Service Unavailable";
    case 504:
        return "Gateway Timeout";
    case 505:
        return "HTTP Version Not Supported";
    case 506:
        return "Variant Also Negotiates";
    case 507:
        return "Insufficient Storage";
    case 508:
        return "Loop Detected";
    case 510:
        return "Not Extended";
    case 511:
        return "Network Authentication Required";
    }

    return "Unknown";
}

sds http_response_encode(http_response_t *response)
{
    sds msg = sdsempty();
    msg = sdscatprintf(msg, "HTTP/1.1 %u %s\r\n", response->status, get_status_description(response->status));

    time_t now = time(NULL);
    struct tm * tm = gmtime(&now);
    char date[100];
    strftime(date, sizeof(date), "%a, %d %B %Y %T GMT", tm);
    msg = sdscatprintf(msg, "Date: %s\r\n", date);
    msg = sdscatprintf(msg, "Content-Length: %zu\r\n", response->content_size);

    dict_iterator *iter = dict_get_iterator(response->headers);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        msg = sdscatprintf(msg, "%s: %s\r\n", (char *)entry->key, (char *)entry->val);
    }
    dict_release_iterator(iter);

    msg = sdscatprintf(msg, "\r\n");
    if (response->content) {
        msg = sdscatlen(msg, response->content, response->content_size);
    }

    return msg;
}

void http_response_release(http_response_t *response)
{
    if (response->headers)
        dict_release(response->headers);
    free(response);
}

const char *http_get_remote_ip(nw_ses *ses, http_request_t *request)
{
    static char ip[NW_SOCK_IP_SIZE];
    const char *x_connecting_ip = http_request_get_header(request, "X-Connecting-IP");
    if (x_connecting_ip)
        return x_connecting_ip;
    const char *x_forward_for = http_request_get_header(request, "X-Forwarded-For");
    if (x_forward_for) {
        sstrncpy(ip, x_forward_for, sizeof(ip));
        char *dot = strchr(ip, ',');
        if (dot) {
            *dot = '0';
        }
        return ip;
    }
    const char *x_proxy_ip = http_request_get_header(request, "X-Proxy-IP");
    if (x_proxy_ip)
        return x_proxy_ip;
    const char *x_real_ip = http_request_get_header(request, "X-Real-IP");
    if (x_real_ip)
        return x_real_ip;


    nw_sock_ip_s(&ses->peer_addr, ip);
    return ip;
}

