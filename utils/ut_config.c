/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/28, create
 */

# include "ut_config.h"
# include "ut_misc.h"

int parse_inetv4_addr(const char *str, struct sockaddr_in *addr)
{
    char *tmp = strdup(str);
    char *ip = strtok(tmp, ": \t");
    if (ip == NULL) {
        return -__LINE__;
    }
    char *port = strtok(NULL, ": \t");
    if (port == NULL) {
        return -__LINE__;
    }
    memset(addr, 0, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons((uint16_t)atoi(port));
    if (inet_aton(ip, &addr->sin_addr) == 0) {
        return -__LINE__;
    }
    free(tmp);

    return 0;
}

int load_cfg_process(json_t *root, const char *key, process_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_object(node)) {
        memset(cfg, 0, sizeof(process_cfg));
        return 0;
    }

    ERR_RET(read_cfg_uint32(node, "file_limit", &cfg->file_limit, false, 0));
    ERR_RET(read_cfg_uint32(node, "core_limit", &cfg->core_limit, false, 0));

    return 0;
}

int load_cfg_log(json_t *root, const char *key, log_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_object(node))
        return -__LINE__;

    ERR_RET(read_cfg_str(node, "path", &cfg->path, NULL));
    ERR_RET(read_cfg_str(node, "flag", &cfg->flag, NULL));

    cfg->shift = 0;
    char *shift;
    ERR_RET(read_cfg_str(node, "shift", &shift, "size"));
    strtolower(shift);
    if (strcmp(shift, "size") == 0) {
        cfg->shift |= DLOG_SHIFT_BY_SIZE;
    } else if (strcmp(shift, "hour") == 0) {
        cfg->shift |= DLOG_SHIFT_BY_HOUR;
    } else if (strcmp(shift, "min") == 0) {
        cfg->shift |= DLOG_SHIFT_BY_MIN;
    } else {
        cfg->shift |= DLOG_SHIFT_BY_DAY;
    }

    bool is_pid;
    bool is_fork;
    ERR_RET(read_cfg_bool(node, "pid", &is_pid, false, true));
    ERR_RET(read_cfg_bool(node, "fork", &is_fork, false, true));
    if (is_pid) {
        cfg->shift |= DLOG_LOG_PID;
    }
    if (is_fork) {
        cfg->shift |= DLOG_USE_FORK;
    }

    ERR_RET(read_cfg_int(node, "max", &cfg->max, false, 100 * 1000 * 1000));
    ERR_RET(read_cfg_int(node, "num", &cfg->num, false, 100));
    ERR_RET(read_cfg_int(node, "keep", &cfg->keep, false, 7));

    return 0;
}

int load_cfg_alert(json_t *root, const char *key, alert_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_object(node))
        return -__LINE__;
    json_t *addr = json_object_get(node, "addr");
    if (!addr || !json_is_string(addr))
        return -__LINE__;
    ERR_RET(parse_inetv4_addr(json_string_value(addr), &cfg->addr));
    ERR_RET(read_cfg_str(node, "host", &cfg->host, NULL));
    return 0;
}

int load_cfg_svr(json_t *root, const char *key, nw_svr_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_object(node))
        return -__LINE__;

    json_t *bind = json_object_get(node, "bind");
    if (!bind)
        return -__LINE__;
    if (json_is_string(bind)) {
        cfg->bind_count = 1;
        cfg->bind_arr = malloc(sizeof(nw_svr_bind));
        if (nw_sock_cfg_parse(json_string_value(bind), &cfg->bind_arr[0].addr, &cfg->bind_arr[0].sock_type) < 0)
            return -__LINE__;
    } else if (json_is_array(bind)) {
        cfg->bind_count = json_array_size(bind);
        if (cfg->bind_count == 0)
            return -__LINE__;
        cfg->bind_arr = malloc(sizeof(nw_svr_bind) * cfg->bind_count);
        for (uint32_t i = 0; i < cfg->bind_count; ++i) {
            json_t *row = json_array_get(bind, i);
            if (!json_is_string(row))
                return -__LINE__;
            if (nw_sock_cfg_parse(json_string_value(row), &cfg->bind_arr[i].addr, &cfg->bind_arr[i].sock_type) < 0)
                return -__LINE__;
        }
    } else {
        return -__LINE__;
    }

    ERR_RET(read_cfg_uint32(node, "max_pkg_size", &cfg->max_pkg_size, true, 0));
    ERR_RET(read_cfg_uint32(node, "buf_limit", &cfg->buf_limit, false, 0));
    ERR_RET(read_cfg_uint32(node, "read_mem", &cfg->read_mem, false, 0));
    ERR_RET(read_cfg_uint32(node, "write_mem", &cfg->write_mem, false, 0));

    return 0;
}

int load_cfg_clt(json_t *root, const char *key, nw_clt_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_object(node))
        return -__LINE__;

    json_t *addr = json_object_get(node, "addr");
    if (!addr || !json_is_string(addr))
        return -__LINE__;
    if (nw_sock_cfg_parse(json_string_value(addr), &cfg->addr, &cfg->sock_type) < 0)
        return -__LINE__;

    ERR_RET(read_cfg_uint32(node, "max_pkg_size", &cfg->max_pkg_size, true, 0));
    ERR_RET(read_cfg_uint32(node, "buf_limit", &cfg->buf_limit, false, 0));
    ERR_RET(read_cfg_uint32(node, "read_mem", &cfg->read_mem, false, 0));
    ERR_RET(read_cfg_uint32(node, "write_mem", &cfg->read_mem, false, 0));
    ERR_RET(read_cfg_real(node, "reconnect_timeout", &cfg->reconnect_timeout, false, 0));

    return 0;
}

int load_cfg_rpc_clt(json_t *root, const char *key, rpc_clt_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_object(node))
        return -__LINE__;

    ERR_RET(read_cfg_str(node, "name", &cfg->name, NULL));

    json_t *addr = json_object_get(node, "addr");
    if (json_is_string(addr)) {
        cfg->addr_count = 1;
        cfg->addr_arr = malloc(sizeof(nw_addr_t));
        if (nw_sock_cfg_parse(json_string_value(addr), &cfg->addr_arr[0], &cfg->sock_type) < 0)
            return -__LINE__;
    } else if (json_is_array(addr)) {
        cfg->addr_count = json_array_size(addr);
        if (cfg->addr_count == 0)
            return -__LINE__;
        cfg->addr_arr = malloc(sizeof(nw_addr_t) * cfg->addr_count);
        for (uint32_t i = 0; i < cfg->addr_count; ++i) {
            json_t *row = json_array_get(addr, i);
            if (!json_is_string(row))
                return -__LINE__;
            if (nw_sock_cfg_parse(json_string_value(row), &cfg->addr_arr[i], &cfg->sock_type) < 0)
                return -__LINE__;
        }
    } else {
        return -__LINE__;
    }

    ERR_RET(read_cfg_uint32(node, "max_pkg_size", &cfg->max_pkg_size, true, 0));
    ERR_RET(read_cfg_uint32(node, "buf_limit", &cfg->buf_limit, false, 0));
    ERR_RET(read_cfg_uint32(node, "read_mem", &cfg->read_mem, false, 0));
    ERR_RET(read_cfg_uint32(node, "write_mem", &cfg->read_mem, false, 0));
    ERR_RET(read_cfg_real(node, "reconnect_timeout", &cfg->reconnect_timeout, false, 0));
    ERR_RET(read_cfg_real(node, "heartbeat_timeout", &cfg->heartbeat_timeout, false, 0));

    return 0;
}

int load_cfg_rpc_svr(json_t *root, const char *key, rpc_svr_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_object(node))
        return -__LINE__;

    json_t *bind = json_object_get(node, "bind");
    if (!bind)
        return -__LINE__;
    if (json_is_string(bind)) {
        cfg->bind_count = 1;
        cfg->bind_arr = malloc(sizeof(nw_svr_bind));
        if (nw_sock_cfg_parse(json_string_value(bind), &cfg->bind_arr[0].addr, &cfg->bind_arr[0].sock_type) < 0)
            return -__LINE__;
    } else if (json_is_array(bind)) {
        cfg->bind_count = json_array_size(bind);
        if (cfg->bind_count == 0)
            return -__LINE__;
        cfg->bind_arr = malloc(sizeof(nw_svr_bind) * cfg->bind_count);
        for (uint32_t i = 0; i < cfg->bind_count; ++i) {
            json_t *row = json_array_get(bind, i);
            if (!json_is_string(row))
                return -__LINE__;
            if (nw_sock_cfg_parse(json_string_value(row), &cfg->bind_arr[i].addr, &cfg->bind_arr[i].sock_type) < 0)
                return -__LINE__;
        }
    } else {
        return -__LINE__;
    }

    ERR_RET(read_cfg_uint32(node, "max_pkg_size", &cfg->max_pkg_size, true, 0));
    ERR_RET(read_cfg_uint32(node, "buf_limit", &cfg->buf_limit, false, 0));
    ERR_RET(read_cfg_uint32(node, "read_mem", &cfg->read_mem, false, 0));
    ERR_RET(read_cfg_uint32(node, "write_mem", &cfg->write_mem, false, 0));
    ERR_RET(read_cfg_bool(node, "heartbeat_check", &cfg->heartbeat_check, false, true));

    return 0;
}

int load_cfg_http_svr(json_t *root, const char *key, http_svr_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_object(node))
        return -__LINE__;

    json_t *bind = json_object_get(node, "bind");
    if (!bind)
        return -__LINE__;
    if (json_is_string(bind)) {
        cfg->bind_count = 1;
        cfg->bind_arr = malloc(sizeof(nw_svr_bind));
        if (nw_sock_cfg_parse(json_string_value(bind), &cfg->bind_arr[0].addr, &cfg->bind_arr[0].sock_type) < 0)
            return -__LINE__;
    } else if (json_is_array(bind)) {
        cfg->bind_count = json_array_size(bind);
        if (cfg->bind_count == 0)
            return -__LINE__;
        cfg->bind_arr = malloc(sizeof(nw_svr_bind) * cfg->bind_count);
        for (uint32_t i = 0; i < cfg->bind_count; ++i) {
            json_t *row = json_array_get(bind, i);
            if (!json_is_string(row))
                return -__LINE__;
            if (nw_sock_cfg_parse(json_string_value(row), &cfg->bind_arr[i].addr, &cfg->bind_arr[i].sock_type) < 0)
                return -__LINE__;
        }
    } else {
        return -__LINE__;
    }

    ERR_RET(read_cfg_uint32(node, "max_pkg_size", &cfg->max_pkg_size, true, 0));
    ERR_RET(read_cfg_uint32(node, "buf_limit", &cfg->buf_limit, false, 0));
    ERR_RET(read_cfg_uint32(node, "read_mem", &cfg->read_mem, false, 0));
    ERR_RET(read_cfg_uint32(node, "write_mem", &cfg->write_mem, false, 0));
    ERR_RET(read_cfg_int(node, "keep_alive", &cfg->keep_alive, false, 3600));

    return 0;
}

int load_cfg_ws_svr(json_t *root, const char *key, ws_svr_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_object(node))
        return -__LINE__;

    json_t *bind = json_object_get(node, "bind");
    if (!bind)
        return -__LINE__;
    if (json_is_string(bind)) {
        cfg->bind_count = 1;
        cfg->bind_arr = malloc(sizeof(nw_svr_bind));
        if (nw_sock_cfg_parse(json_string_value(bind), &cfg->bind_arr[0].addr, &cfg->bind_arr[0].sock_type) < 0)
            return -__LINE__;
    } else if (json_is_array(bind)) {
        cfg->bind_count = json_array_size(bind);
        if (cfg->bind_count == 0)
            return -__LINE__;
        cfg->bind_arr = malloc(sizeof(nw_svr_bind) * cfg->bind_count);
        for (uint32_t i = 0; i < cfg->bind_count; ++i) {
            json_t *row = json_array_get(bind, i);
            if (!json_is_string(row))
                return -__LINE__;
            if (nw_sock_cfg_parse(json_string_value(row), &cfg->bind_arr[i].addr, &cfg->bind_arr[i].sock_type) < 0)
                return -__LINE__;
        }
    } else {
        return -__LINE__;
    }

    ERR_RET(read_cfg_uint32(node, "max_pkg_size", &cfg->max_pkg_size, true, 0));
    ERR_RET(read_cfg_uint32(node, "buf_limit", &cfg->buf_limit, false, 0));
    ERR_RET(read_cfg_uint32(node, "read_mem", &cfg->read_mem, false, 0));
    ERR_RET(read_cfg_uint32(node, "write_mem", &cfg->write_mem, false, 0));
    ERR_RET(read_cfg_int(node, "keep_alive", &cfg->keep_alive, false, 3600));
    ERR_RET(read_cfg_str(node, "protocol", &cfg->protocol, "chat"));
    ERR_RET(read_cfg_str(node, "origin", &cfg->origin, ""));

    return 0;
}

int load_cfg_cli_svr(json_t *root, const char *key, cli_svr_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_string(node))
        return -__LINE__;
    ERR_RET_LN(nw_sock_cfg_parse(json_string_value(node), &cfg->addr, &cfg->sock_type));

    return 0;
}

int load_cfg_inetv4_list(json_t *root, const char *key, inetv4_list *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_array(node))
        return -__LINE__;

    cfg->count = json_array_size(node);
    cfg->arr = malloc(sizeof(struct sockaddr_in) * cfg->count);
    for (size_t i = 0; i < cfg->count; ++i) {
        json_t *row = json_array_get(node, i);
        if (!json_is_string(row)) {
            return -__LINE__;
        }
        ERR_RET(parse_inetv4_addr(json_string_value(row), &cfg->arr[i]));
    }

    return 0;
}

int load_cfg_mysql(json_t *root,  const char *key, mysql_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_object(node))
        return -__LINE__;

    ERR_RET(read_cfg_str(node, "host", &cfg->host, NULL));
    ERR_RET(read_cfg_int(node, "port", &cfg->port, false, 3306));
    ERR_RET(read_cfg_str(node, "user", &cfg->user, NULL));
    ERR_RET(read_cfg_str(node, "pass", &cfg->pass, NULL));
    ERR_RET(read_cfg_str(node, "name", &cfg->name, NULL));
    ERR_RET(read_cfg_str(node, "charset", &cfg->charset, "utf8"));

    return 0;
}

int load_cfg_kafka_consumer(json_t *root, const char *key, kafka_consumer_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_object(node))
        return -__LINE__;

    ERR_RET(read_cfg_str(node, "brokers", &cfg->brokers, NULL));
    ERR_RET(read_cfg_str(node, "topic", &cfg->topic, NULL));
    ERR_RET(read_cfg_int(node, "partition", &cfg->partition, false, 0));
    ERR_RET(read_cfg_int(node, "limit", &cfg->limit, false, 1000));
    ERR_RET(read_cfg_int64(node, "offset", &cfg->offset, false, 0));

    return 0;
}

int load_cfg_redis_sentinel(json_t *root, const char *key, redis_sentinel_cfg *cfg)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_object(node))
        return -__LINE__;

    ERR_RET(read_cfg_str(node, "name", &cfg->name, NULL));
    ERR_RET(read_cfg_int(node, "db", &cfg->db, false, 0));

    json_t *addr = json_object_get(node, "addr");
    if (!addr || !json_is_array(addr))
        return -__LINE__;
    cfg->addr_count = json_array_size(addr);
    if (cfg->addr_count == 0)
        return -__LINE__;
    cfg->addr_arr = malloc(sizeof(redis_addr) * cfg->addr_count);
    if (cfg->addr_arr == NULL)
        return -__LINE__;
    for (size_t i = 0; i < cfg->addr_count; ++i) {
        json_t *row = json_array_get(addr, i);
        if (!row || !json_is_string(row))
            return -__LINE__;
        if (redis_addr_cfg_parse(json_string_value(row), &cfg->addr_arr[i]) < 0)
            return -__LINE__;
    }

    return 0;
}

int read_cfg_str(json_t *root, const char *key, char **val, const char *default_val)
{
    json_t *node = json_object_get(root, key);
    if (!node && default_val) {
        *val = strdup(default_val);
        if (*val == NULL)
            return -__LINE__;
        return 0;
    }
    if (!node || !json_is_string(node))
        return -__LINE__;
    *val = strdup(json_string_value(node));
    if (*val == NULL)
        return -__LINE__;

    return 0;
}

int read_cfg_mpd(json_t *root, const char *key, mpd_t **val, const char *default_val)
{
    json_t *node = json_object_get(root, key);
    if (!node && default_val) {
        *val = decimal(default_val, 0);
        if (*val == NULL)
            return -__LINE__;
        return 0;
    }
    if (!node || !json_is_string(node))
        return -__LINE__;
    *val = decimal(json_string_value(node), 0);
    if (*val == NULL)
        return -__LINE__;

    return 0;
}

int read_cfg_int(json_t *root, const char *key, int *val, bool required, int default_val)
{
    json_t *node = json_object_get(root, key);
    if (!node) {
        if (required) {
            return -__LINE__;
        } else {
            *val = default_val;
            return 0;
        }
    }
    if (!json_is_integer(node))
        return -__LINE__;
    *val = json_integer_value(node);

    return 0;
}

int read_cfg_int32(json_t *root, const char *key, int32_t *val, bool required, int32_t default_val)
{
    json_t *node = json_object_get(root, key);
    if (!node) {
        if (required) {
            return -__LINE__;
        } else {
            *val = default_val;
            return 0;
        }
    }
    if (!json_is_integer(node))
        return -__LINE__;
    *val = (int32_t)json_integer_value(node);

    return 0;
}

int read_cfg_int64(json_t *root, const char *key, int64_t *val, bool required, int64_t default_val)
{
    json_t *node = json_object_get(root, key);
    if (!node) {
        if (required) {
            return -__LINE__;
        } else {
            *val = default_val;
            return 0;
        }
    }
    if (!json_is_integer(node))
        return -__LINE__;
    *val = (int64_t)json_integer_value(node);

    return 0;
}

int read_cfg_uint32(json_t *root, const char *key, uint32_t *val, bool required, uint32_t default_val)
{
    json_t *node = json_object_get(root, key);
    if (!node) {
        if (required) {
            return -__LINE__;
        } else {
            *val = default_val;
            return 0;
        }
    }
    if (!json_is_integer(node))
        return -__LINE__;
    *val = (uint32_t)json_integer_value(node);

    return 0;
}

int read_cfg_uint64(json_t *root, const char *key, uint64_t *val, bool required, uint64_t default_val)
{
    json_t *node = json_object_get(root, key);
    if (!node) {
        if (required) {
            return -__LINE__;
        } else {
            *val = default_val;
            return 0;
        }
    }
    if (!json_is_integer(node))
        return -__LINE__;
    *val = (uint64_t)json_integer_value(node);

    return 0;
}

int read_cfg_real(json_t *root, const char *key, double *val, bool required, double default_val)
{
    json_t *node = json_object_get(root, key);
    if (!node) {
        if (required) {
            return -__LINE__;
        } else {
            *val = default_val;
            return 0;
        }
    }
    if (json_is_integer(node)) {
        *val = json_integer_value(node);
    } else if (json_is_real(node)) {
        *val = json_real_value(node);
    } else {
        return -__LINE__;
    }

    return 0;
}

int read_cfg_bool(json_t *root, const char *key, bool *val, bool required, bool default_val)
{
    json_t *node = json_object_get(root, key);
    if (!node) {
        if (required) {
            return -__LINE__;
        } else {
            *val = default_val;
            return 0;
        }
    }
    if (!json_is_boolean(node))
        return -__LINE__;
    *val = json_boolean_value(node);

    return 0;
}

