/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/24, create
 */

# include "rh_config.h"
# include "rh_reader.h"

json_t *get_user_balance_history(MYSQL *conn, uint32_t user_id,
        const char *asset, const char *business, uint64_t start_time, uint64_t end_time, size_t offset, size_t limit)
{
    size_t asset_len = strlen(asset);
    char _asset[2 * asset_len + 1];
    mysql_real_escape_string(conn, _asset, asset, strlen(asset));

    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT `time`, `business`, `change`, `balance`, `detail` FROM `balance_history_%u` WHERE `user_id` = %u AND asset = '%s'",
            user_id % HISTORY_HASH_NUM, user_id, _asset);

    size_t business_len = strlen(business);
    if (business_len > 0) {
        char _business[2 * business_len + 1];
        mysql_real_escape_string(conn, _business, business, business_len);
        sql = sdscatprintf(sql ," AND `business` = '%s'", _business);
    }

    if (start_time) {
        sql = sdscatprintf(sql, " AND `time` >= %"PRIu64, start_time);
    }
    if (end_time) {
        sql = sdscatprintf(sql, " AND `time` < %"PRIu64, end_time);
    }

    sql = sdscatprintf(sql, " ORDER BY `id` DESC");
    if (offset) {
        sql = sdscatprintf(sql, " LIMIT %zu %zu", limit, offset);
    } else {
        sql = sdscatprintf(sql, " LIMIT %zu", limit);
    }

    log_trace("exec sql: %s", sql);
    int ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret != 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        sdsfree(sql);
        return NULL;
    }
    sdsfree(sql);

    MYSQL_RES *result = mysql_store_result(conn);
    size_t num_rows = mysql_num_rows(result);
    json_t *records = json_array();
    for (size_t i = 0; i < num_rows; ++i) {
        json_t *record = json_object();
        MYSQL_ROW row = mysql_fetch_row(result);
        double timestamp = strtod(row[0], NULL);
        json_object_set_new(record, "time", json_real(timestamp));
        json_object_set_new(record, "business", json_string(row[1]));
        json_object_set_new(record, "change", json_string(row[2]));
        json_object_set_new(record, "balance", json_string(row[3]));
        json_object_set_new(record, "detail", json_string(row[4]));

        json_array_append_new(records, record);
    }
    mysql_free_result(result);

    return records;
}

json_t *get_user_order_finished(MYSQL *conn, uint32_t user_id,
        const char *market, uint64_t start_time, uint64_t end_time, size_t offset, size_t limit)
{
    size_t market_len = strlen(market);
    char _market[2 * market_len + 1];
    mysql_real_escape_string(conn, _market, market, market_len);

    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT `id`, `create_time`, `finish_time`, `user_id`, `market`, `t`, `side`, `price`, `amount`, "
            "`taker_fee`, `maker_fee`, `deal_stock`, `deal_money`, `deal_fee` FROM `order_history_%u` WHERE `user_id` = %u "
            "AND `market` = '%s'", user_id % HISTORY_HASH_NUM, user_id, _market);
    if (start_time) {
        sql = sdscatprintf(sql, "AND `create_time` >= %"PRIu64, start_time);
    }
    if (end_time) {
        sql = sdscatprintf(sql, "AND `create_time` < %"PRIu64, end_time);
    }

    sql = sdscatprintf(sql, " ORDER BY `id` DESC");
    if (offset) {
        sql = sdscatprintf(sql, " LIMIT %zu %zu", limit, offset);
    } else {
        sql = sdscatprintf(sql, " LIMIT %zu", limit);
    }

    log_trace("exec sql: %s", sql);
    int ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret != 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        sdsfree(sql);
        return NULL;
    }
    sdsfree(sql);

    MYSQL_RES *result = mysql_store_result(conn);
    size_t num_rows = mysql_num_rows(result);
    json_t *records = json_array();
    for (size_t i = 0; i < num_rows; ++i) {
        json_t *record = json_object();
        MYSQL_ROW row = mysql_fetch_row(result);
        uint64_t order_id = strtoull(row[0], NULL, 0);
        json_object_set_new(record, "id", json_integer(order_id));
        double ctime = strtod(row[1], NULL);
        json_object_set_new(record, "ctime", json_real(ctime));
        double ftime = strtod(row[2], NULL);
        json_object_set_new(record, "ftime", json_real(ftime));
        uint32_t user_id = strtoul(row[3], NULL, 0);
        json_object_set_new(record, "user", json_integer(user_id));
        json_object_set_new(record, "market", json_string(row[4]));
        uint32_t type = atoi(row[5]);
        json_object_set_new(record, "type", json_integer(type));
        uint32_t side = atoi(row[6]);
        json_object_set_new(record, "side", json_integer(side));
        json_object_set_new(record, "price", json_string(row[7]));
        json_object_set_new(record, "amount", json_string(row[8]));
        json_object_set_new(record, "taker_fee", json_string(row[9]));
        json_object_set_new(record, "maker_fee", json_string(row[10]));
        json_object_set_new(record, "deal_stock", json_string(row[11]));
        json_object_set_new(record, "deal_money", json_string(row[12]));
        json_object_set_new(record, "deal_fee", json_string(row[13]));

        json_array_append_new(records, record);
    }
    mysql_free_result(result);

    return records;
}

json_t *get_order_deal_details(MYSQL *conn, uint64_t order_id)
{
    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT `time`, `deal_order_id`, `role`, `amount`, `price`, `deal`, `fee` "
            "FROM `deal_history_%u` where `id` = %"PRIu64" ORDER BY `id` DESC", (uint32_t)(order_id % HISTORY_HASH_NUM), order_id);

    log_trace("exec sql: %s", sql);
    int ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret != 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        sdsfree(sql);
        return NULL;
    }
    sdsfree(sql);

    MYSQL_RES *result = mysql_store_result(conn);
    size_t num_rows = mysql_num_rows(result);
    json_t *records = json_array();
    for (size_t i = 0; i < num_rows; ++i) {
        json_t *record = json_object();
        MYSQL_ROW row = mysql_fetch_row(result);
        double timestamp = strtod(row[0], NULL);
        json_object_set_new(record, "time", json_real(timestamp));
        uint64_t deal_order_id = strtoull(row[1], NULL, 0);
        json_object_set_new(record, "deal_order_id", json_integer(deal_order_id));
        int role = atoi(row[2]);
        json_object_set_new(record, "role", json_integer(role));
        json_object_set_new(record, "amount", json_string(row[3]));
        json_object_set_new(record, "price", json_string(row[4]));
        json_object_set_new(record, "deal", json_string(row[5]));
        json_object_set_new(record, "fee", json_string(row[6]));

        json_array_append_new(records, record);
    }
    mysql_free_result(result);

    return records;
}

json_t *get_finished_order_detail(MYSQL *conn, uint64_t order_id)
{
    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT `id`, `create_time`, `finish_time`, `user_id`, `market`, `t`, `side`, `price`, `amount`, "
            "`taker_fee`, `maker_fee`, `deal_stock`, `deal_money`, `deal_fee` FROM `order_detail_%u` "
            "WHERE `id` = %"PRIu64, (uint32_t)(order_id % HISTORY_HASH_NUM), order_id);

    log_trace("exec sql: %s", sql);
    int ret = mysql_real_query(conn, sql, sdslen(sql));
    if (ret != 0) {
        log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn), mysql_error(conn));
        sdsfree(sql);
        return NULL;
    }
    sdsfree(sql);

    MYSQL_RES *result = mysql_store_result(conn);
    size_t num_rows = mysql_num_rows(result);
    if (num_rows == 0) {
        return json_null();
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    json_t *detail = json_object();
    json_object_set_new(detail, "id", json_integer(order_id));
    double ctime = strtod(row[1], NULL);
    json_object_set_new(detail, "ctime", json_real(ctime));
    double ftime = strtod(row[2], NULL);
    json_object_set_new(detail, "ftime", json_real(ftime));
    uint32_t user_id = strtoul(row[3], NULL, 0);
    json_object_set_new(detail, "user", json_integer(user_id));
    json_object_set_new(detail, "market", json_string(row[4]));
    uint32_t type = atoi(row[5]);
    json_object_set_new(detail, "type", json_integer(type));
    uint32_t side = atoi(row[6]);
    json_object_set_new(detail, "side", json_integer(side));
    json_object_set_new(detail, "price", json_string(row[7]));
    json_object_set_new(detail, "amount", json_string(row[8]));
    json_object_set_new(detail, "taker_fee", json_string(row[9]));
    json_object_set_new(detail, "maker_fee", json_string(row[10]));
    json_object_set_new(detail, "deal_stock", json_string(row[11]));
    json_object_set_new(detail, "deal_money", json_string(row[12]));
    json_object_set_new(detail, "deal_fee", json_string(row[13]));
    mysql_free_result(result);

    return detail;
}

