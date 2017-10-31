/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/26, create
 */

# include <stdbool.h>
# include <openssl/sha.h>  

# include "ut_log.h"
# include "ut_misc.h"
# include "ut_base64.h"
# include "ut_ws_svr.h"

struct ws_frame {
    uint8_t     fin;
    uint8_t     opcode;
    uint64_t    payload_len;
    void        *payload;
};

struct clt_info {
    nw_ses      *ses;
    void        *privdata;
    double      last_activity;
    struct      http_parser parser;
    sds         field;
    bool        field_set;
    sds         value;
    bool        value_set;
    bool        upgrade;
    sds         remote;
    sds         url;
    sds         message;
    http_request_t *request;
    struct ws_frame frame;
};

static int on_http_message_begin(http_parser* parser)
{
    struct clt_info *info = parser->data;
    if (info->request)
        http_request_release(info->request);
    info->request = http_request_new();
    if (info->request == NULL) {
        return -__LINE__;
    }

    return 0;
}

static int send_hand_shake_reply(nw_ses *ses, char *protocol, const char *key)
{
    unsigned char hash[20];
    sds data = sdsnew(key);
    data = sdscat(data, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    SHA1((const unsigned char *)data, sdslen(data), hash);
    sdsfree(data);

    sds b4message;
    base64_encode(hash, sizeof(hash), &b4message);

    http_response_t *response = http_response_new();
    http_response_set_header(response, "Upgrade", "websocket");
    http_response_set_header(response, "Connection", "Upgrade");
    http_response_set_header(response, "Sec-WebSocket-Accept", b4message);
    if (protocol) {
        http_response_set_header(response, "Sec-WebSocket-Protocol", protocol);
    }
    response->status = 101;

    sds message = http_response_encode(response);
    nw_ses_send(ses, message, sdslen(message));

    sdsfree(message);
    sdsfree(b4message);

    return 0;
}

static bool is_good_protocol(const char *protocol_list, const char *protocol)
{
    char *tmp = strdup(protocol_list);
    char *pch = strtok(tmp, ", ");
    while (pch != NULL) {
        if (strcmp(pch, protocol) == 0) {
            free(tmp);
            return true;
        }
        pch = strtok(NULL, ", ");
    }
    free(tmp);
    return false;
}

static bool is_good_origin(const char *origin, const char *require)
{
    size_t origin_len  = strlen(origin);
    size_t require_len = strlen(require);
    if (origin_len < require_len)
        return false;
    if (memcmp(origin + (origin_len - require_len), require, require_len) != 0)
        return false;
    return true;
}

static int on_http_message_complete(http_parser* parser)
{
    struct clt_info *info = parser->data;
    ws_svr *svr = ws_svr_from_ses(info->ses);
    info->request->version_major = parser->http_major;
    info->request->version_minor = parser->http_minor;
    info->request->method = parser->method;

    dict_entry *entry;
    dict_iterator *iter = dict_get_iterator(info->request->headers);
    while ((entry = dict_next(iter)) != NULL) {
        log_trace("Header: %s: %s", (char *)entry->key, (char *)entry->val);
    }
    dict_release_iterator(iter);

    if (info->request->method != HTTP_GET)
        goto error;
    if (http_request_get_header(info->request, "Host") == NULL)
        goto error;
    double version = info->request->version_major + info->request->version_minor * 0.1;
    if (version < 1.1)
        goto error;
    const char *upgrade = http_request_get_header(info->request, "Upgrade");
    if (upgrade == NULL || strcasecmp(upgrade, "websocket") != 0)
        goto error;
    const char *connection = http_request_get_header(info->request, "Connection");
    if (connection == NULL)
        goto error;
    else {
        bool found_upgrade = false;
        int count;
        sds *tokens = sdssplitlen(connection, strlen(connection), ",", 1, &count); 
        if (tokens == NULL)
            goto error;
        for (int i = 0; i < count; i++) {
            sds token = tokens[i];
            sdstrim(token, " ");
            if (strcasecmp(token, "Upgrade") == 0) {
                found_upgrade = true;
                break;
            }
        }
        sdsfreesplitres(tokens, count);
        if (!found_upgrade)
            goto error;
    }
    const char *ws_version = http_request_get_header(info->request, "Sec-WebSocket-Version");
    if (ws_version == NULL || strcmp(ws_version, "13") != 0)
        goto error;
    const char *ws_key = http_request_get_header(info->request, "Sec-WebSocket-Key");
    if (ws_key == NULL)
        goto error;
    const char *protocol_list = http_request_get_header(info->request, "Sec-WebSocket-Protocol");
    if (protocol_list && !is_good_protocol(protocol_list, svr->protocol))
        goto error;
    if (strlen(svr->origin) > 0) {
        const char *origin = http_request_get_header(info->request, "Origin");
        if (origin == NULL || !is_good_origin(origin, svr->origin))
            goto error;
    }

    if (svr->type.on_privdata_alloc) {
        info->privdata = svr->type.on_privdata_alloc(svr);
        if (info->privdata == NULL)
            goto error;
    }
    info->upgrade = true;
    info->remote = sdsnew(http_get_remote_ip(info->ses, info->request));
    info->url = sdsnew(info->request->url);
    if (svr->type.on_upgrade) {
        svr->type.on_upgrade(info->ses, info->remote);
    }
    if (protocol_list) {
        send_hand_shake_reply(info->ses, svr->protocol, ws_key);
    } else {
        send_hand_shake_reply(info->ses, NULL, ws_key);
    }

    return 0;

error:
    ws_svr_close_clt(ws_svr_from_ses(info->ses), info->ses);
    return -1;
}

static int on_http_url(http_parser* parser, const char* at, size_t length)
{
    struct clt_info *info = parser->data;
    if (info->request->url)
        sdsfree(info->request->url);
    info->request->url = sdsnewlen(at, length);

    return 0;
}

static int on_http_header_field(http_parser* parser, const char* at, size_t length)
{
    struct clt_info *info = parser->data;
    info->field_set = true;
    if (info->field == NULL) {
        info->field = sdsnewlen(at, length);
    } else {
        info->field = sdscpylen(info->field, at, length);
    }

    return 0;
}

static int on_http_header_value(http_parser* parser, const char* at, size_t length)
{
    struct clt_info *info = parser->data;
    info->value_set = true;
    if (info->value == NULL) {
        info->value = sdsnewlen(at, length);
    } else {
        info->value = sdscpylen(info->value, at, length);
    }

    if (info->field_set && info->value_set) {
        http_request_set_header(info->request, info->field, info->value);
        info->field_set = false;
        info->value_set = false;
    }

    return 0;
}

static int on_http_body(http_parser* parser, const char* at, size_t length)
{
    struct clt_info *info = parser->data;
    info->request->body = sdsnewlen(at, length);

    return 0;
}

static bool is_good_opcode(uint8_t opcode)
{
    static uint8_t good_list[] = { 0x0, 0x1, 0x2, 0x8, 0x9, 0xa };
    for (size_t i = 0; i < sizeof(good_list); ++i) {
        if (opcode == good_list[i])
            return true;
    }
    return false;
}

static int decode_pkg(nw_ses *ses, void *data, size_t max)
{
    struct clt_info *info = ses->privdata;
    if (!info->upgrade) {
        return max;
    }

    if (max < 2)
        return 0;

    uint8_t *p = data;
    size_t pkg_size = 0;
    memset(&info->frame, 0, sizeof(info->frame));
    info->frame.fin = p[0] & 0x80;
    info->frame.opcode = p[0] & 0x0f;
    if (!is_good_opcode(info->frame.opcode))
        return -1;
    uint8_t mask = p[1] & 0x80;
    if (mask == 0)
        return -1;

    uint8_t len = p[1] & 0x7f;
    if (len < 126) {
        pkg_size = 2;
        info->frame.payload_len = len;
    } else if (len == 126) {
        pkg_size = 2 + 2;
        if (max < pkg_size)
            return 0;
        info->frame.payload_len = be16toh(*(uint16_t *)(p + 2));
    } else if (len == 127) {
        pkg_size = 2 + 8;
        if (max < pkg_size)
            return 0;
        info->frame.payload_len = be64toh(*(uint64_t *)(p + 2));
    }

    uint8_t masks[4];
    memcpy(masks, p + pkg_size, sizeof(masks));
    pkg_size += sizeof(masks);
    info->frame.payload = p + pkg_size;
    pkg_size += info->frame.payload_len;
    if (max < pkg_size)
        return 0;

    p = info->frame.payload;
    for (size_t i = 0; i < info->frame.payload_len; ++i) {
        p[i] = p[i] ^ masks[i & 3];
    }

    return pkg_size;
}

static void on_error_msg(nw_ses *ses, const char *msg)
{
    log_error("peer: %s: %s", nw_sock_human_addr(&ses->peer_addr), msg);
}

static void on_new_connection(nw_ses *ses)
{
    log_trace("new connection from: %s", nw_sock_human_addr(&ses->peer_addr));
    struct clt_info *info = ses->privdata;
    memset(info, 0, sizeof(struct clt_info));
    info->ses = ses;
    info->last_activity = current_timestamp();
    http_parser_init(&info->parser, HTTP_REQUEST);
    info->parser.data = info;
}

static void on_connection_close(nw_ses *ses)
{
    log_trace("connection %s close", nw_sock_human_addr(&ses->peer_addr));
    struct clt_info *info = ses->privdata;
    struct ws_svr *svr = ws_svr_from_ses(ses);
    if (info->upgrade) {
        if (svr->type.on_close) {
            svr->type.on_close(ses, info->remote);
        }
        if (svr->type.on_privdata_free) {
            svr->type.on_privdata_free(svr, info->privdata);
        }
    }
}

static void *on_privdata_alloc(void *svr)
{
    ws_svr *w_svr = ((nw_svr *)svr)->privdata;
    return nw_cache_alloc(w_svr->privdata_cache);
}

static void on_privdata_free(void *svr, void *privdata)
{
    struct clt_info *info = privdata;
    if (info->field) {
        sdsfree(info->field);
    }
    if (info->value) {
        sdsfree(info->value);
    }
    if (info->remote) {
        sdsfree(info->remote);
    }
    if (info->url) {
        sdsfree(info->url);
    }
    if (info->message) {
        sdsfree(info->message);
    }
    if (info->request) {
        http_request_release(info->request);
    }
    ws_svr *w_svr = ((nw_svr *)svr)->privdata;
    nw_cache_free(w_svr->privdata_cache, privdata);
}

static int send_reply(nw_ses *ses, uint8_t opcode, void *payload, size_t payload_len)
{
    if (payload == NULL)
        payload_len = 0;

    static void *buf;
    static size_t buf_size = 1024;
    if (buf == NULL) {
        buf = malloc(1024);
        if (buf == NULL)
            return -1;
    }
    size_t require_len = 10 + payload_len;
    if (buf_size < require_len) {
        void *new = realloc(buf, require_len);
        if (new == NULL)
            return -1;
        buf = new;
        buf_size = require_len;
    }

    size_t pkg_len = 0;
    uint8_t *p = buf;
    p[0] = 0;
    p[0] |= 0x1 << 7;
    p[0] |= opcode;
    p[1] = 0;
    if (payload_len < 126) {
        uint8_t len = payload_len;
        p[1] |= len;
        pkg_len = 2;
    } else if (payload_len <= 0xffff) {
        p[1] |= 126;
        uint16_t len = htobe16((uint16_t)payload_len);
        memcpy(p + 2, &len, sizeof(len));
        pkg_len = 2 + sizeof(len);
    } else {
        p[1] |= 127;
        uint64_t len = htobe64(payload_len);
        memcpy(p + 2, &len, sizeof(len));
        pkg_len = 2 + sizeof(len);
    }

    if (payload) {
        memcpy(p + pkg_len, payload, payload_len);
        pkg_len += payload_len;
    }

    return nw_ses_send(ses, buf, pkg_len);
}

static int send_pong_message(nw_ses *ses)
{
    return send_reply(ses, 0xa, NULL, 0);
}

static void on_recv_pkg(nw_ses *ses, void *data, size_t size)
{
    struct clt_info *info = ses->privdata;
    ws_svr *svr = ws_svr_from_ses(ses);
    info->last_activity = current_timestamp();
    if (!info->upgrade) {
        size_t nparsed = http_parser_execute(&info->parser, &svr->settings, data, size);
        if (!info->parser.upgrade && nparsed != size) {
            log_error("peer: %s http parse error: %s (%s)", nw_sock_human_addr(&ses->peer_addr),
                    http_errno_description(HTTP_PARSER_ERRNO(&info->parser)),
                    http_errno_name(HTTP_PARSER_ERRNO(&info->parser)));
            nw_svr_close_clt(svr->raw_svr, ses);
        }
        return;
    }

    switch (info->frame.opcode) {
    case 0x8:
        nw_svr_close_clt(svr->raw_svr, ses);
        return;
    case 0x9:
        send_pong_message(ses);
        return;
    case 0xa:
        return;
    }

    if (info->message == NULL)
        info->message = sdsempty();
    info->message = sdscatlen(info->message, info->frame.payload, info->frame.payload_len);
    if (info->frame.fin) {
        int ret = svr->type.on_message(ses, info->remote, info->url, info->message, sdslen(info->message));
        if (ses->id != 0) {
            if (ret < 0) {
                nw_svr_close_clt(svr->raw_svr, ses);
            } else {
                sdsfree(info->message);
                info->message = NULL;
            }
        }
    }
}

static void on_timer(nw_timer *timer, void *privdata)
{
   ws_svr *svr = privdata;
   double now = current_timestamp();

   nw_ses *curr = svr->raw_svr->clt_list_head;
   nw_ses *next;
   while (curr) {
       next = curr->next;
       struct clt_info *info = curr->privdata;
       if (now - info->last_activity > svr->keep_alive) {
           log_error("peer: %s: last_activity: %f, idle too long", nw_sock_human_addr(&curr->peer_addr), info->last_activity);
           nw_svr_close_clt(svr->raw_svr, curr);
       }
       curr = next;
   }
}

ws_svr *ws_svr_create(ws_svr_cfg *cfg, ws_svr_type *type)
{
    if (type->on_message == NULL)
        return NULL;
    if (type->on_privdata_alloc && !type->on_privdata_free)
        return NULL;

    ws_svr *svr = malloc(sizeof(ws_svr));
    memset(svr, 0, sizeof(ws_svr));

    nw_svr_cfg raw_cfg;
    memset(&raw_cfg, 0, sizeof(raw_cfg));
    raw_cfg.bind_count = cfg->bind_count;
    raw_cfg.bind_arr = cfg->bind_arr;
    raw_cfg.max_pkg_size = cfg->max_pkg_size;
    raw_cfg.buf_limit = cfg->buf_limit;
    raw_cfg.read_mem = cfg->read_mem;
    raw_cfg.write_mem = cfg->write_mem;

    nw_svr_type st;
    memset(&st, 0, sizeof(st));
    st.decode_pkg = decode_pkg;
    st.on_error_msg = on_error_msg;
    st.on_new_connection = on_new_connection;
    st.on_connection_close = on_connection_close;
    st.on_recv_pkg = on_recv_pkg;
    st.on_privdata_alloc = on_privdata_alloc;
    st.on_privdata_free = on_privdata_free;

    svr->raw_svr = nw_svr_create(&raw_cfg, &st, svr);
    if (svr->raw_svr == NULL) {
        free(svr);
        return NULL;
    }

    memset(&svr->settings, 0, sizeof(http_parser_settings));
    svr->settings.on_message_begin = on_http_message_begin;
    svr->settings.on_url = on_http_url;
    svr->settings.on_header_field = on_http_header_field;
    svr->settings.on_header_value = on_http_header_value;
    svr->settings.on_body = on_http_body;
    svr->settings.on_message_complete = on_http_message_complete;

    svr->keep_alive = cfg->keep_alive;
    svr->protocol = strdup(cfg->protocol);
    svr->origin   = strdup(cfg->origin);
    svr->privdata_cache = nw_cache_create(sizeof(struct clt_info));
    memcpy(&svr->type, type, sizeof(ws_svr_type));

    if (cfg->keep_alive > 0) {
        nw_timer_set(&svr->timer, 60, true, on_timer, svr);
        nw_timer_start(&svr->timer);
    }

    return svr;
}

int ws_svr_start(ws_svr *svr)
{
    int ret = nw_svr_start(svr->raw_svr);
    if (ret < 0)
        return ret;

    return 0;
}

int ws_svr_stop(ws_svr *svr)
{
    int ret = nw_svr_stop(svr->raw_svr);
    if (ret < 0)
        return ret;

    return 0;
}

ws_svr *ws_svr_from_ses(nw_ses *ses)
{
    return ((nw_svr *)ses->svr)->privdata;
}

void *ws_ses_privdata(nw_ses *ses)
{
    struct clt_info *info = ses->privdata;
    return info->privdata;
}

int ws_send_text(nw_ses *ses, char *message)
{
    return send_reply(ses, 0x1, message, strlen(message));
}

int ws_send_binary(nw_ses *ses, void *data, size_t size)
{
    return send_reply(ses, 0x2, data, size);
}

static int broadcast_message(ws_svr *svr, uint8_t opcode, void *data, size_t size)
{
    nw_ses *curr = svr->raw_svr->clt_list_head;
    while (curr) {
        nw_ses *next = curr->next;
        struct clt_info *info = curr->privdata;
        if (info->upgrade) {
            int ret = send_reply(curr, opcode, data, size);
            if (ret < 0)
                return ret;
        }
        curr = next;
    }

    return 0;
}

int ws_svr_broadcast_text(ws_svr *svr, char *message)
{
    return broadcast_message(svr, 0x1, message, strlen(message));
}

int ws_svr_broadcast_binary(ws_svr *svr, void *data, size_t size)
{
    return broadcast_message(svr, 0x2, data, size);
}

void ws_svr_close_clt(ws_svr *svr, nw_ses *ses)
{
    nw_svr_close_clt(svr->raw_svr, ses);
}

void ws_svr_release(ws_svr *svr)
{
    nw_svr_release(svr->raw_svr);
    nw_timer_stop(&svr->timer);
    nw_cache_release(svr->privdata_cache);
    free(svr->protocol);
    free(svr);
}

