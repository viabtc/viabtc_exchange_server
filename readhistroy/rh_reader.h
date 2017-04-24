/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/24, create
 */

# ifndef _RH_READER_H_
# define _RH_READER_H_

# include "rh_config.h"

json_t *get_user_balance_history(MYSQL *conn, uint32_t user_id,
        const char *asset, uint64_t start_time, uint64_t end_time, size_t offset, size_t limit);
json_t *get_user_order_finished(MYSQL *conn, uint32_t user_id,
        const char *market, uint64_t start_time, uint64_t end_time, size_t offset, size_t limit);
json_t *get_order_deal_details(MYSQL *conn, uint64_t order_id);

# endif

