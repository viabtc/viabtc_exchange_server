/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/16, create
 */

# ifndef _MP_MESSAGE_H_
# define _MP_MESSAGE_H_

# define MARKET_SIDE_SELL   1
# define MARKET_SIDE_BUY    2
# define MARKET_DEALS_MAX   10000
# define MARKET_NAME_MAX    12

# include "mp_config.h"

int init_message(void);
bool market_exist(const char *market);

json_t *get_market_status(const char *market, int period);
json_t *get_market_kline_sec(const char *market, time_t start, time_t end, int interval);
json_t *get_market_kline_min(const char *market, time_t start, time_t end, int interval);
json_t *get_market_kline_hour(const char *market, time_t start, time_t end, int interval);
json_t *get_market_kline_day(const char *market, time_t start, time_t end, int interval);
json_t *get_market_kline_week(const char *market, time_t start, time_t end, int interval);
json_t *get_market_deals(const char *market, int limit, uint64_t last_id);

# endif

