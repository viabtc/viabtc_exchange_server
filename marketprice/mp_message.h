/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/16, create
 */

# ifndef _MP_MESSAGE_H_
# define _MP_MESSAGE_H_

# include "mp_config.h"

# define MARKET_DEALS_MAX   10000
# define MARKET_NAME_MAX    12

int init_message(void);
bool market_exist(const char *market);

json_t *get_market_status(const char *market, int period);
json_t *get_market_status_today(const char *market);
json_t *get_market_kline_sec(const char *market, time_t start, time_t end, int interval);
json_t *get_market_kline_min(const char *market, time_t start, time_t end, int interval);
json_t *get_market_kline_hour(const char *market, time_t start, time_t end, int interval);
json_t *get_market_kline_day(const char *market, time_t start, time_t end, int interval);
json_t *get_market_kline_week(const char *market, time_t start, time_t end, int interval);
json_t *get_market_kline_month(const char *market, time_t start, time_t end, int interval);
json_t *get_market_deals(const char *market, int limit, uint64_t last_id);
mpd_t  *get_market_last_price(const char *market);

# endif

