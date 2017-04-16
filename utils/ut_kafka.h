/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/15, create
 */

# ifndef _UT_KAFKA_H_
# define _UT_KAFKA_H_

# include <stddef.h>
# include <stdint.h>
# include <stdbool.h>
# include <pthread.h>
# include <librdkafka/rdkafka.h>

# include "nw_evt.h"
# include "ut_sds.h"
# include "ut_list.h"

typedef void (*kafka_message_callback)(sds message, int64_t offset);

typedef struct kafka_consumer_cfg {
    char    *brokers;
    char    *topic;
    int     partition;
    int     limit;
    int64_t offset;
} kafka_consumer_cfg;

typedef struct kafka_consumer_t {
    ev_io ev;
    struct ev_loop *loop;
    int pipefd[2];
    bool running;
    bool shutdown;
    pthread_mutex_t lock;
    pthread_t thread;
    rd_kafka_conf_t *conf;
    rd_kafka_t *rk;
    rd_kafka_topic_t *rkt;
    int32_t partition;
    list_t *list;
    int limit;
    kafka_message_callback callback;
} kafka_consumer_t;

kafka_consumer_t *kafka_consumer_create(kafka_consumer_cfg *cfg, kafka_message_callback callback);
void kafka_consumer_release(kafka_consumer_t *consumer);

# endif

