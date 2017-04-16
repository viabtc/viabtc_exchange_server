/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/16, create
 */

# include "mp_config.h"
# include "mp_message.h"

static kafka_consumer_t *consumer_deals;

static void on_deals_message(sds message, int64_t offset)
{
    log_debug("deals message: %s, offset: %"PRIi64, message, offset);
}

int init_message(void)
{
    consumer_deals = kafka_consumer_create(&settings.deals, on_deals_message);
    if (consumer_deals == NULL) {
        return -__LINE__;
    }

    return 0;
}

