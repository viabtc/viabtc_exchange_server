/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/27, create
 */

# include <stdlib.h>
# include <string.h>
# include <stdarg.h>
# include <errno.h>

# include "ut_log.h"
# include "ut_redis.h"

static void append_node(redis_sentinel_t *context, redis_sentinel_node *node)
{
    if (context->list == NULL) {
        context->list = node;
        return;
    }
    redis_sentinel_node *curr = context->list;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = node;
    node->prev = curr;
}

static void move_to_front(redis_sentinel_t *context, redis_sentinel_node *node)
{
    if (node == context->list)
        return;
    node->prev->next = node->next;
    if (node->next) {
        node->next->prev = node->prev;
    }
    node->next = context->list;
    node->prev = NULL;
    context->list->prev = node;
    context->list = node;
}

redis_sentinel_t *redis_sentinel_create(redis_sentinel_cfg *cfg)
{
    redis_sentinel_t *context = malloc(sizeof(redis_sentinel_t));
    if (context == NULL)
        return NULL;
    memset(context, 0, sizeof(redis_sentinel_t));
    context->db = cfg->db;
    context->name = strdup(cfg->name);
    if (context->name == NULL) {
        free(context);
        return NULL;
    }

    for (uint32_t i = 0; i < cfg->addr_count; ++i) {
        redis_sentinel_node *node = malloc(sizeof(redis_sentinel_node));
        if (node == NULL) {
            redis_sentinel_release(context);
            return NULL;
        }
        memset(node, 0, sizeof(redis_sentinel_node));
        node->addr.host = strdup(cfg->addr_arr[i].host);
        node->addr.port = cfg->addr_arr[i].port;
        if (node->addr.host == NULL) {
            free(node);
            redis_sentinel_release(context);
            return NULL;
        }
        append_node(context, node);
    }

    return context;
}

void redis_sentinel_release(redis_sentinel_t *context)
{
    redis_sentinel_node *curr = context->list;
    redis_sentinel_node *next;
    while (curr) {
        next = curr->next;
        free(curr->addr.host);
        free(curr);
        curr = next;
    }
    free(context);
}

int redis_sentinel_get_master_addr(redis_sentinel_t *context, redis_addr *addr)
{
    redis_sentinel_node *curr = context->list;
    while (curr) {
        struct timeval timeout = { 3, 0 };
        redisContext *redis = redisConnectWithTimeout(curr->addr.host, curr->addr.port, timeout);
        if (redis == NULL || redis->err) {
            if (redis) {
                redisFree(redis);
            }
            curr = curr->next;
            continue;
        }
        redisReply *reply = redisCommand(redis, "SENTINEL get-master-addr-by-name %s", context->name);
        if (reply == NULL || reply->type != REDIS_REPLY_ARRAY || reply->elements != 2) {
            if (reply) {
                freeReplyObject(reply);
                redisFree(redis);
            }
            curr = curr->next;
            continue;
        }

        move_to_front(context, curr);
        addr->host = strdup(reply->element[0]->str);
        addr->port = atoi(reply->element[1]->str);
        freeReplyObject(reply);
        redisFree(redis);

        return 0;
    }

    return -1;
}

static char *get_slave_info(size_t elements, redisReply **element, char *key)
{
    for (size_t i = 0; i < elements; i += 2){
        if (strcmp(element[i]->str, key) == 0) {
            return element[i + 1]->str;
        }
    }
    return NULL;
}

int redis_sentinel_get_slave_addr(redis_sentinel_t *context, redis_addr *addr)
{
    redis_sentinel_node *curr = context->list;
    while (curr) {
        struct timeval timeout = { 3, 0 };
        redisContext *redis = redisConnectWithTimeout(curr->addr.host, curr->addr.port, timeout);
        if (redis == NULL || redis->err) {
            if (redis) {
                redisFree(redis);
            }
            curr = curr->next;
            continue;
        }
        redisReply *reply = redisCommand(redis, "SENTINEL slaves %s", context->name);
        if (reply == NULL || reply->type != REDIS_REPLY_ARRAY) {
            if (reply) {
                freeReplyObject(reply);
                redisFree(redis);
            }
            curr = curr->next;
            continue;
        }

        for (size_t i = 0; i < reply->elements; ++i) {
            const char *flags = get_slave_info(reply->element[i]->elements, reply->element[i]->element, "flags");
            if (flags == NULL || strstr(flags, "disconnected")) {
                continue;
            }
            const char *host = get_slave_info(reply->element[i]->elements, reply->element[i]->element, "ip");
            if (host == NULL) {
                continue;
            }
            const char *port = get_slave_info(reply->element[i]->elements, reply->element[i]->element, "port");
            if (port == NULL) {
                continue;
            }

            move_to_front(context, curr);
            addr->host = strdup(host);
            addr->port = atoi(port);
            freeReplyObject(reply);
            redisFree(redis);

            return 0;
        }

        curr = curr->next;
    }

    return -1;
}

redisContext *redis_sentinel_connect_master(redis_sentinel_t *context)
{
    for (int i = 0; i < 3; ++i) {
        redis_addr addr;
        if (redis_sentinel_get_master_addr(context, &addr) < 0)
            return NULL;

        struct timeval timeout = { 3, 0 };
        redisContext *redis = redisConnectWithTimeout(addr.host, addr.port, timeout);
        if (redis == NULL || redis->err) {
            if (redis) {
                redisFree(redis);
            }
            free(addr.host);
            return NULL;
        }
        free(addr.host);
        redisSetTimeout(redis, timeout);

        redisReply *reply = redisCommand(redis, "ROLE");
        if (reply == NULL || reply->type != REDIS_REPLY_ARRAY) {
            if (reply) {
                freeReplyObject(reply);
            }
            redisFree(redis);
            return NULL;
        }
        if (strcmp(reply->element[0]->str, "master") != 0) {
            freeReplyObject(reply);
            redisFree(redis);
            continue;
        }
        freeReplyObject(reply);

        if (context->db > 0) {
            reply = redisCommand(redis, "SELECT %d", context->db);
            if (redis == NULL || reply->type == REDIS_REPLY_ERROR) {
                if (reply) {
                    freeReplyObject(reply);
                }
                redisFree(redis);
                return NULL;
            }
            freeReplyObject(reply);
        }

        return redis;
    }

    return NULL;
}

redisContext *redis_sentinel_connect_slave(redis_sentinel_t *context)
{
    for (int i = 0; i < 3; ++i) {
        redis_addr addr;
        if (redis_sentinel_get_slave_addr(context, &addr) < 0)
            return NULL;

        struct timeval timeout = { 3, 0 };
        redisContext *redis = redisConnectWithTimeout(addr.host, addr.port, timeout);
        if (redis == NULL || redis->err) {
            if (redis) {
                redisFree(redis);
            }
            free(addr.host);
            return NULL;
        }
        free(addr.host);
        redisSetTimeout(redis, timeout);

        redisReply *reply = redisCommand(redis, "ROLE");
        if (reply == NULL || reply->type != REDIS_REPLY_ARRAY) {
            if (reply) {
                freeReplyObject(reply);
            }
            redisFree(redis);
            return NULL;
        }
        if (strcmp(reply->element[0]->str, "slave") != 0) {
            freeReplyObject(reply);
            redisFree(redis);
            continue;
        }
        freeReplyObject(reply);

        if (context->db > 0) {
            reply = redisCommand(redis, "SELECT %d", context->db);
            if (redis == NULL || reply->type == REDIS_REPLY_ERROR) {
                if (reply) {
                    freeReplyObject(reply);
                }
                redisFree(redis);
                return NULL;
            }
            freeReplyObject(reply);
        }

        return redis;
    }

    return NULL;
}

int redis_addr_cfg_parse(const char *cfg, redis_addr *addr)
{
    char *sep = strchr(cfg, ':');
    if (sep == NULL)
        return -1;
    addr->port = atoi(sep + 1);
    if (addr->port <= 0)
        return -1;
    addr->host = malloc(sep - cfg + 1);
    if (addr->host == NULL)
        return -1;
    memcpy(addr->host, cfg, sep - cfg);
    addr->host[sep - cfg] = '\0';
    return 0;
}

void *redisCmd(redisContext *c, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    redisReply *reply = redisvCommand(c, format, ap);
    va_end(ap);

    if (reply == NULL) {
        log_error("redisCommand fail: %d: %s", c->err, strerror(errno));
        return NULL;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        log_error("redisCommand error: %s", reply->str);
        freeReplyObject(reply);
        return NULL;
    }
    return reply;
}

void *redisRawCmd(redisContext *c, const char *cmd)
{
    redisReply *reply = redisCommand(c, cmd);
    if (reply == NULL) {
        log_error("redisCommand: %s fail: %d: %s", cmd, c->err, strerror(errno));
        return NULL;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        log_error("redisCommand: %s error: %s", cmd, reply->str);
        freeReplyObject(reply);
        return NULL;
    }
    return reply;
}

