/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/16, create
 */

# include "mp_config.h"
# include "mp_message.h"

static redis_sentinel_t *redis;
static kafka_consumer_t *deals;

static void on_deals_message(sds message, int64_t offset)
{
    log_debug("deals message: %s, offset: %"PRIi64, message, offset);
}

int init_message(void)
{
    redis = redis_sentinel_create(&settings.redis);
    if (redis == NULL)
        return -__LINE__;

    deals = kafka_consumer_create(&settings.deals, on_deals_message);
    if (deals == NULL) {
        return -__LINE__;
    }

    return 0;
}

