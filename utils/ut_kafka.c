/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/15, create
 */

# include <stdlib.h>
# include <unistd.h>

# include "nw_sock.h"
# include "ut_log.h"
# include "ut_kafka.h"

typedef struct message_t {
    sds message;
    int64_t offset;
} message_t;

static void free_message(void *value)
{
    message_t *m = value;
    sdsfree(m->message);
    free(m);
}

static void on_logger(const rd_kafka_t *rk, int level, const char *fac, const char *buf)
{
    log_error("RDKAFKA-%i-%s: %s: %s\n", level, fac, rk ? rd_kafka_name(rk) : NULL, buf);
}

static void *thread_routine(void *data)
{
    kafka_consumer_t *consumer = data;
    pthread_mutex_lock(&consumer->lock);
    consumer->running = true;
    pthread_mutex_unlock(&consumer->lock);

    while (consumer->shutdown == false) {
        if (consumer->list->len >= consumer->limit) {
            usleep(100 * 1000);
            continue;
        }

        rd_kafka_poll(consumer->rk, 0);
        rd_kafka_message_t *rkmessage = rd_kafka_consume(consumer->rkt, consumer->partition, 100);
        if (!rkmessage)
            continue;
        if (rkmessage->err) {
            if (rkmessage->err != RD_KAFKA_RESP_ERR__PARTITION_EOF) {
                log_error("Consume error for topic \"%s\" [%"PRId32"] offset %"PRId64": %s", rd_kafka_topic_name(rkmessage->rkt),
                        rkmessage->partition, rkmessage->offset, rd_kafka_message_errstr(rkmessage));
            }
        } else {
            struct message_t *m = malloc(sizeof(message_t));
            m->message = sdsnewlen(rkmessage->payload, rkmessage->len);
            m->offset = rkmessage->offset;
            pthread_mutex_lock(&consumer->lock);
            list_add_node_head(consumer->list, m);
            write(consumer->pipefd[1], " ", 1);
            pthread_mutex_unlock(&consumer->lock);
        }
        rd_kafka_message_destroy(rkmessage);
    }

    rd_kafka_consume_stop(consumer->rkt, consumer->partition);
    return data;
}

static void on_can_read(struct ev_loop *loop, ev_io *watcher, int events)
{
    kafka_consumer_t *consumer = (kafka_consumer_t *)watcher;
    for (;;) {
        char c;
        int ret = read(consumer->pipefd[0], &c, 1);
        if (ret < 0)
            break;
    }

    for (;;) {
        pthread_mutex_lock(&consumer->lock);
        if (consumer->list->len == 0) {
            pthread_mutex_unlock(&consumer->lock);
            break;
        }
        list_node *node = list_tail(consumer->list);
        message_t *m = node->value;
        consumer->callback(m->message, m->offset);
        list_del(consumer->list, node);
        pthread_mutex_unlock(&consumer->lock);
    }
}

kafka_consumer_t *kafka_consumer_create(kafka_consumer_cfg *cfg, kafka_message_callback callback)
{
    kafka_consumer_t *consumer = malloc(sizeof(kafka_consumer_t));
    if (consumer == NULL)
        return NULL;
    memset(consumer, 0, sizeof(kafka_consumer_t));

    nw_loop_init();
    consumer->loop = nw_default_loop;
    consumer->callback = callback;

    if (pipe(consumer->pipefd) != 0) {
        free(consumer);
        return NULL;
    }

    nw_sock_set_nonblock(consumer->pipefd[0]);
    nw_sock_set_nonblock(consumer->pipefd[1]);
    ev_io_init(&consumer->ev, on_can_read, consumer->pipefd[0], EV_READ);
    ev_io_start(consumer->loop, &consumer->ev);

    list_type lt;
    memset(&lt, 0, sizeof(lt));
    lt.free = free_message;
    consumer->list = list_create(&lt);
    if (consumer->list == NULL) {
        kafka_consumer_release(consumer);
        return NULL;
    }
    consumer->limit = cfg->limit;

    char errstr[1024];
    consumer->partition = cfg->partition;
    consumer->conf = rd_kafka_conf_new();
    rd_kafka_conf_set_log_cb(consumer->conf, on_logger);
    consumer->rk = rd_kafka_new(RD_KAFKA_CONSUMER, consumer->conf, errstr, sizeof(errstr));
    if (consumer->rk == NULL) {
        log_error("Failed to create new consumer: %s", errstr);
        log_stderr("Failed to create new consumer: %s", errstr);
        kafka_consumer_release(consumer);
        return NULL;
    }
    consumer->conf = NULL;
    if (rd_kafka_brokers_add(consumer->rk, cfg->brokers) == 0) {
        log_error("No valid brokers specified: %s", rd_kafka_err2str(rd_kafka_last_error()));
        log_stderr("No valid brokers specified: %s", rd_kafka_err2str(rd_kafka_last_error()));
        kafka_consumer_release(consumer);
        return NULL;
    }
    consumer->rkt = rd_kafka_topic_new(consumer->rk, cfg->topic, NULL);
    if (consumer->rkt == NULL) {
        log_error("Failed to create topic object: %s", rd_kafka_err2str(rd_kafka_last_error()));
        log_stderr("Failed to create topic object: %s", rd_kafka_err2str(rd_kafka_last_error()));
        kafka_consumer_release(consumer);
        return NULL;
    }
    if (rd_kafka_consume_start(consumer->rkt, cfg->partition, cfg->offset) == -1) {
        log_error("Failed to start consumer: %s", rd_kafka_err2str(rd_kafka_last_error()));
        log_stderr("Failed to start consumer: %s", rd_kafka_err2str(rd_kafka_last_error()));
        kafka_consumer_release(consumer);
        return NULL;
    }
    if (pthread_mutex_init(&consumer->lock, NULL) != 0) {
        kafka_consumer_release(consumer);
        return NULL;
    }
    if (pthread_create(&consumer->thread, NULL, thread_routine, consumer) != 0) {
        kafka_consumer_release(consumer);
        return NULL;
    }

    return consumer;
}

void kafka_consumer_release(kafka_consumer_t *consumer)
{
    pthread_mutex_lock(&consumer->lock);
    if (consumer->shutdown) {
        pthread_mutex_unlock(&consumer->lock);
        return;
    }
    consumer->shutdown = true;
    pthread_mutex_unlock(&consumer->lock);

    if (consumer->running) {
        pthread_join(consumer->thread, NULL);
        consumer->running = false;
    }

    ev_io_stop(consumer->loop, &consumer->ev);
    close(consumer->pipefd[0]);
    close(consumer->pipefd[1]);
    if (consumer->list) {
        list_release(consumer->list);
    }
    if (consumer->conf) {
        rd_kafka_conf_destroy(consumer->conf);
    }
    if (consumer->rk) {
        rd_kafka_destroy(consumer->rk);
    }
    if (consumer->rkt) {
        rd_kafka_topic_destroy(consumer->rkt);
    }
}

