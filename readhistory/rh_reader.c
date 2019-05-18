/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/24, create
 */

# include "rh_config.h"
# include "rh_reader.h"
# include "ut_decimal.h"

//30,16                  FM 99999 99999  99999 99999  99999 99990.9999 9999 9999 9999
const char *NUM_FMT = "FM999999999999999999999999999990.0999999999999999";

json_t *get_user_balance_history(struct db_connection *conn, uint32_t user_id,
        const char *asset, const char *business, uint64_t start_time, uint64_t end_time, size_t offset, size_t limit)
{

    if (is_select_from_mysql(conn)) {
        sds sql = sdsempty();
        sql = sdscatprintf(sql,
                           "SELECT `time`, `asset`, `business`, `change`, `balance`, `detail` FROM `balance_history_%u` WHERE `user_id` = %u",
                           user_id % HISTORY_HASH_NUM, user_id);

        size_t asset_len = strlen(asset);
        if (asset_len > 0) {
            char _asset[2 * asset_len + 1];
            mysql_real_escape_string(conn->mysql, _asset, asset, strlen(asset));
            sql = sdscatprintf(sql, " AND `asset` = '%s'", _asset);
        }
        size_t business_len = strlen(business);
        if (business_len > 0) {
            char _business[2 * business_len + 1];
            mysql_real_escape_string(conn->mysql, _business, business, strlen(business));
            sql = sdscatprintf(sql, " AND `business` = '%s'", _business);
        }

        if (start_time) {
            sql = sdscatprintf(sql, " AND `time` >= %"PRIu64, start_time);
        }
        if (end_time) {
            sql = sdscatprintf(sql, " AND `time` < %"PRIu64, end_time);
        }

        sql = sdscatprintf(sql, " ORDER BY `id` DESC");
        if (offset) {
            sql = sdscatprintf(sql, " LIMIT %zu, %zu", offset, limit);
        } else {
            sql = sdscatprintf(sql, " LIMIT %zu", limit);
        }

        log_trace("exec sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_fatal("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return NULL;
        }
        sdsfree(sql);

        MYSQL_RES *result = mysql_store_result(conn->mysql);
        size_t num_rows = mysql_num_rows(result);
        json_t *records = json_array();
        for (size_t i = 0; i < num_rows; ++i) {
            json_t *record = json_object();
            MYSQL_ROW row = mysql_fetch_row(result);
            double timestamp = strtod(row[0], NULL);
            json_object_set_new(record, "time", json_real(timestamp));
            json_object_set_new(record, "asset", json_string(row[1]));
            json_object_set_new(record, "business", json_string(row[2]));
            json_object_set_new(record, "change", json_string(rstripzero(row[3])));
            json_object_set_new(record, "balance", json_string(rstripzero(row[4])));
            json_t *detail = json_loads(row[5], 0, NULL);
            if (detail == NULL || !json_is_object(detail)) {
                if (detail) {
                    json_decref(detail);
                }
                detail = json_object();
            }
            json_object_set_new(record, "detail", detail);

            json_array_append_new(records, record);
        }
        mysql_free_result(result);
        return records;
    }
    // else SELECT FROM postgre
    sds base_sql = sdsempty();
    base_sql = sdscatprintf(base_sql, "SELECT extract(epoch FROM \"time\")  AS \"time\", asset, business, "
                                      "to_char(\"change\",'%s') as \"change\", "
                                      "to_char(balance,'%s') as balance, "
                                      "detail "
                                      "FROM balance_history WHERE user_id = %u", NUM_FMT, NUM_FMT, user_id);
    size_t asset_len = strlen(asset);
    if (asset_len > 0) {
        //TODO escape_string
        base_sql = sdscatprintf(base_sql, " AND asset = '%s'::text", asset);
    }
    size_t business_len = strlen(business);
    if (business_len > 0) {
        //TODO escape_string
        base_sql = sdscatprintf(base_sql, " AND business = '%s'::text", business);
    }

    if (start_time) {
        base_sql = sdscatprintf(base_sql, " AND \"time\" >=  to_timestamp(%"PRIu64")", start_time);
    }
    if (end_time) {
        base_sql = sdscatprintf(base_sql, " AND \"time\" < to_timestamp(%"PRIu64")", end_time);
    }

    base_sql = sdscatprintf(base_sql, " ORDER BY id DESC");
    if (offset) {
        base_sql = sdscatprintf(base_sql, " OFFSET %zu LIMIT %zu", offset, limit);
    } else {
        base_sql = sdscatprintf(base_sql, " LIMIT %zu", limit);
    }

    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT array_to_json( array_agg(row_to_json(t)) ) FROM ( %s ) t", base_sql);
    sdsfree(base_sql);

    log_trace("exec postgresql sql: %s", sql);
    PGresult* res = PQexec(conn->postgresql, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        sdsfree(sql);
        return NULL;
    }
    sdsfree(sql);
    size_t num_rows = PQntuples(res);
    if (num_rows != 1) {
        PQclear(res);
        return json_array();
    }
    const char* result_str = PQgetvalue(res, 0, 0);
    json_t *records = json_loads(result_str, 0, NULL);
    PQclear(res);
    if (records == NULL) {
        // 'dirty' but it's fast and no need change sql array_to_json( array_agg(row_to_json(t)) )
        // NOTE may be need change sql
        return json_array();
    }
    return records;
}

json_t *get_user_order_finished(struct db_connection *conn, uint32_t user_id,
        const char *market, int side, uint64_t start_time, uint64_t end_time, size_t offset, size_t limit)
{
    if (is_select_from_mysql(conn)) {
        size_t market_len = strlen(market);
        char _market[2 * market_len + 1];
        mysql_real_escape_string(conn->mysql, _market, market, market_len);

        sds sql = sdsempty();
        sql = sdscatprintf(sql,
                           "SELECT `id`, `create_time`, `finish_time`, `user_id`, `market`, `source`, `t`, `side`, `price`, `amount`, "
                           "`taker_fee`, `maker_fee`, `deal_stock`, `deal_money`, `deal_fee` FROM `order_history_%u` WHERE `user_id` = %u "
                           "AND `market` = '%s'", user_id % HISTORY_HASH_NUM, user_id, _market);
        if (side) {
            sql = sdscatprintf(sql, " AND `side` = %d", side);
        }
        if (start_time) {
            sql = sdscatprintf(sql, " AND `create_time` >= %"PRIu64, start_time);
        }
        if (end_time) {
            sql = sdscatprintf(sql, " AND `create_time` < %"PRIu64, end_time);
        }

        sql = sdscatprintf(sql, " ORDER BY `id` DESC");
        if (offset) {
            sql = sdscatprintf(sql, " LIMIT %zu, %zu", offset, limit);
        } else {
            sql = sdscatprintf(sql, " LIMIT %zu", limit);
        }

        log_trace("exec sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_fatal("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return NULL;
        }
        sdsfree(sql);

        MYSQL_RES *result = mysql_store_result(conn->mysql);
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
            json_object_set_new(record, "source", json_string(row[5]));
            uint32_t type = atoi(row[6]);
            json_object_set_new(record, "type", json_integer(type));
            uint32_t side = atoi(row[7]);
            json_object_set_new(record, "side", json_integer(side));
            json_object_set_new(record, "price", json_string(rstripzero(row[8])));
            json_object_set_new(record, "amount", json_string(rstripzero(row[9])));
            json_object_set_new(record, "taker_fee", json_string(rstripzero(row[10])));
            json_object_set_new(record, "maker_fee", json_string(rstripzero(row[11])));
            json_object_set_new(record, "deal_stock", json_string(rstripzero(row[12])));
            json_object_set_new(record, "deal_money", json_string(rstripzero(row[13])));
            json_object_set_new(record, "deal_fee", json_string(rstripzero(row[14])));

            json_array_append_new(records, record);
        }
        mysql_free_result(result);

        return records;
    }
    // else SELECT FROM postgre
    sds base_sql = sdsempty();
    base_sql = sdscatprintf(base_sql, "SELECT id, extract(epoch FROM create_time) as ctime, "
                                 "extract(epoch FROM finish_time) as ftime, "
                                 "user_id as user, market, source, order_type as type, side, "
                                 "to_char(price,'%s') as price, "
                                 "to_char(amount,'%s') as amount, "
                                 "to_char(taker_fee,'%s') as taker_fee, "
                                 "to_char(maker_fee,'%s') as maker_fee, "
                                 "to_char(deal_stock,'%s') as deal_stock, "
                                 "to_char(deal_money,'%s') as deal_money, "
                                 "to_char(deal_fee,'%s') as  deal_fee "
                                 "FROM order_history WHERE user_id = %u",
                            NUM_FMT, NUM_FMT, NUM_FMT, NUM_FMT,
                            NUM_FMT, NUM_FMT, NUM_FMT, user_id);
    if (side) {
        base_sql = sdscatprintf(base_sql, " AND side = %d", side);
    }
    if (start_time) {
        base_sql = sdscatprintf(base_sql, " AND create_time >= to_timestamp(%"PRIu64")", start_time);
    }
    if (end_time) {
        base_sql = sdscatprintf(base_sql, " AND create_time < to_timestamp(%"PRIu64")", end_time);
    }

    base_sql = sdscatprintf(base_sql, " ORDER BY id DESC");
    if (offset) {
        base_sql = sdscatprintf(base_sql, " OFFSET %zu LIMIT %zu", offset, limit);
    } else {
        base_sql = sdscatprintf(base_sql, " LIMIT %zu", limit);
    }

    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT array_to_json( array_agg(row_to_json(t)) ) FROM ( %s ) t", base_sql);
    sdsfree(base_sql);

    log_trace("exec postgresql sql: %s", sql);
    PGresult* res = PQexec(conn->postgresql, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        sdsfree(sql);
        return NULL;
    }
    sdsfree(sql);
    size_t num_rows = PQntuples(res);
    if (num_rows != 1) {
        PQclear(res);
        return json_array();
    }
    const char* result_str = PQgetvalue(res, 0, 0);
    json_t *records = json_loads(result_str, 0, NULL);
    PQclear(res);
    if (records == NULL) {
        // 'dirty' but it's fast and no need change sql array_to_json( array_agg(row_to_json(t)) )
        // NOTE may be need change sql
        return json_array();
    }
    return records;

}

json_t *get_order_deal_details(struct db_connection *conn, uint64_t order_id, size_t offset, size_t limit)
{
    if (is_select_from_mysql(conn)) {
        sds sql = sdsempty();
        sql = sdscatprintf(sql,
                           "SELECT `time`, `user_id`, `deal_id`, `role`, `price`, `amount`, `deal`, `fee`, `deal_order_id` "
                           "FROM `deal_history_%u` where `order_id` = %"PRIu64" ORDER BY `id` DESC",
                           (uint32_t)(order_id % HISTORY_HASH_NUM), order_id);
        if (offset) {
            sql = sdscatprintf(sql, " LIMIT %zu, %zu", offset, limit);
        } else {
            sql = sdscatprintf(sql, " LIMIT %zu", limit);
        }

        log_trace("exec sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_fatal("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return NULL;
        }
        sdsfree(sql);

        MYSQL_RES *result = mysql_store_result(conn->mysql);
        size_t num_rows = mysql_num_rows(result);
        json_t *records = json_array();
        for (size_t i = 0; i < num_rows; ++i) {
            json_t *record = json_object();
            MYSQL_ROW row = mysql_fetch_row(result);
            double timestamp = strtod(row[0], NULL);
            json_object_set_new(record, "time", json_real(timestamp));
            uint32_t user_id = strtoul(row[1], NULL, 0);
            json_object_set_new(record, "user", json_integer(user_id));
            uint64_t deal_id = strtoull(row[2], NULL, 0);
            json_object_set_new(record, "id", json_integer(deal_id));
            int role = atoi(row[3]);
            json_object_set_new(record, "role", json_integer(role));

            json_object_set_new(record, "price", json_string(rstripzero(row[4])));
            json_object_set_new(record, "amount", json_string(rstripzero(row[5])));
            json_object_set_new(record, "deal", json_string(rstripzero(row[6])));
            json_object_set_new(record, "fee", json_string(rstripzero(row[7])));

            uint64_t deal_order_id = strtoull(row[8], NULL, 0);
            json_object_set_new(record, "deal_order_id", json_integer(deal_order_id));

            json_array_append_new(records, record);
        }
        mysql_free_result(result);

        return records;
    }
    // else SELECT FROM postgre
    sds base_sql = sdsempty();
    base_sql = sdscatprintf(base_sql,"SELECT extract(epoch FROM \"time\") AS \"time\","
                                   "user_id AS user,"
                                   "deal_id AS id,"
                                   "\"role\","
                                   "to_char(price,'%s') as price,"
                                   "to_char(amount,'%s') as amount,"
                                   "to_char(deal,'%s') as deal,"
                                   "to_char(fee,'%s') as fee,"
                                   "deal_order_id,"
                                   "to_char(deal_fee,'%s') as  deal_fee "
                                "FROM deal_history WHERE order_id = %"PRIu64" ORDER BY id DESC",
                            NUM_FMT, NUM_FMT, NUM_FMT, NUM_FMT, NUM_FMT, order_id);

    if (offset) {
        base_sql = sdscatprintf(base_sql, " OFFSET %zu LIMIT %zu", offset, limit);
    } else {
        base_sql = sdscatprintf(base_sql, " LIMIT %zu", limit);
    }

    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT array_to_json( array_agg(row_to_json(t)) ) FROM ( %s ) t", base_sql);
    sdsfree(base_sql);

    log_trace("exec postgresql sql: %s", sql);
    PGresult* res = PQexec(conn->postgresql, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        sdsfree(sql);
        return NULL;
    }
    sdsfree(sql);
    size_t num_rows = PQntuples(res);
    if (num_rows != 1) {
        PQclear(res);
        return json_array();
    }
    const char* result_str = PQgetvalue(res, 0, 0);
    json_t *records = json_loads(result_str, 0, NULL);
    PQclear(res);
    if (records == NULL) {
        // 'dirty' but it's fast and no need change sql array_to_json( array_agg(row_to_json(t)) )
        // NOTE may be need change sql
        return json_array();
    }
    return records;
}

json_t *get_finished_order_detail(struct db_connection *conn, uint64_t order_id)
{
    if (is_select_from_mysql(conn)) {
        sds sql = sdsempty();
        sql = sdscatprintf(sql,
                           "SELECT `id`, `create_time`, `finish_time`, `user_id`, `market`, `source`, `t`, `side`, `price`, `amount`, "
                           "`taker_fee`, `maker_fee`, `deal_stock`, `deal_money`, `deal_fee` FROM `order_detail_%u` "
                           "WHERE `id` = %"PRIu64, (uint32_t)(order_id % HISTORY_HASH_NUM), order_id);

        log_trace("exec sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_fatal("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return NULL;
        }
        sdsfree(sql);

        MYSQL_RES *result = mysql_store_result(conn->mysql);
        size_t num_rows = mysql_num_rows(result);
        if (num_rows == 0) {
            mysql_free_result(result);
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
        json_object_set_new(detail, "source", json_string(row[5]));
        uint32_t type = atoi(row[6]);
        json_object_set_new(detail, "type", json_integer(type));
        uint32_t side = atoi(row[7]);
        json_object_set_new(detail, "side", json_integer(side));
        json_object_set_new(detail, "price", json_string(rstripzero(row[8])));
        json_object_set_new(detail, "amount", json_string(rstripzero(row[9])));
        json_object_set_new(detail, "taker_fee", json_string(rstripzero(row[10])));
        json_object_set_new(detail, "maker_fee", json_string(rstripzero(row[11])));
        json_object_set_new(detail, "deal_stock", json_string(rstripzero(row[12])));
        json_object_set_new(detail, "deal_money", json_string(rstripzero(row[13])));
        json_object_set_new(detail, "deal_fee", json_string(rstripzero(row[14])));
        mysql_free_result(result);

        return detail;
    }
    // else SELECT FROM postgre
    sds base_sql = sdsempty();
    base_sql = sdscatprintf(base_sql, "SELECT id, extract(epoch FROM create_time) as ctime, "
                                 "extract(epoch FROM finish_time) as ftime, "
                                 "user_id as user, market, source, order_type as type, side, "
                                 "to_char(price,'%s') as price, "
                                 "to_char(amount,'%s') as amount, "
                                 "to_char(taker_fee,'%s') as taker_fee, "
                                 "to_char(maker_fee,'%s') as maker_fee, "
                                 "to_char(deal_stock,'%s') as deal_stock, "
                                 "to_char(deal_money,'%s') as deal_money, "
                                 "to_char(deal_fee,'%s') as  deal_fee "
                                 "FROM order_detail WHERE id = %"PRIu64,
                            NUM_FMT, NUM_FMT, NUM_FMT, NUM_FMT,  NUM_FMT, NUM_FMT, NUM_FMT, order_id);


    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT row_to_json(t)  FROM ( %s ) t", base_sql);
    sdsfree(base_sql);

    log_trace("exec postgresql sql: %s", sql);
    PGresult* res = PQexec(conn->postgresql, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        sdsfree(sql);
        return NULL;
    }
    sdsfree(sql);
    size_t num_rows = PQntuples(res);
    if (num_rows == 0) {
        PQclear(res);
        return json_null();
    }
    const char* result_str = PQgetvalue(res, 0, 0);
    json_t *records = json_loads(result_str, 0, NULL);
    PQclear(res);
    return records;
}

json_t *get_market_user_deals(struct db_connection *conn, uint32_t user_id, const char *market, size_t offset, size_t limit)
{
    if (is_select_from_mysql(conn)) {
        size_t market_len = strlen(market);
        char _market[2 * market_len + 1];
        mysql_real_escape_string(conn->mysql, _market, market, market_len);

        sds sql = sdsempty();
        sql = sdscatprintf(sql,
                           "SELECT `time`, `user_id`, `deal_id`, `side`, `role`, `price`, `amount`,"
                           " `deal`, `fee`, `deal_order_id`, `market` "
                           "FROM `user_deal_history_%u` where `user_id` = %u AND `market` = '%s' ORDER BY `id` DESC",
                           user_id % HISTORY_HASH_NUM, user_id, _market);
        if (offset) {
            sql = sdscatprintf(sql, " LIMIT %zu, %zu", offset, limit);
        } else {
            sql = sdscatprintf(sql, " LIMIT %zu", limit);
        }

        log_trace("exec sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_fatal("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return NULL;
        }
        sdsfree(sql);

        MYSQL_RES *result = mysql_store_result(conn->mysql);
        size_t num_rows = mysql_num_rows(result);
        json_t *records = json_array();
        for (size_t i = 0; i < num_rows; ++i) {
            json_t *record = json_object();
            MYSQL_ROW row = mysql_fetch_row(result);
            double timestamp = strtod(row[0], NULL);
            json_object_set_new(record, "time", json_real(timestamp));
            uint32_t user_id = strtoul(row[1], NULL, 0);
            json_object_set_new(record, "user", json_integer(user_id));
            uint64_t deal_id = strtoull(row[2], NULL, 0);
            json_object_set_new(record, "id", json_integer(deal_id));
            int side = atoi(row[3]);
            json_object_set_new(record, "side", json_integer(side));
            int role = atoi(row[4]);
            json_object_set_new(record, "role", json_integer(role));

            json_object_set_new(record, "price", json_string(rstripzero(row[5])));
            json_object_set_new(record, "amount", json_string(rstripzero(row[6])));
            json_object_set_new(record, "deal", json_string(rstripzero(row[7])));
            json_object_set_new(record, "fee", json_string(rstripzero(row[8])));

            uint64_t deal_order_id = strtoull(row[9], NULL, 0);
            json_object_set_new(record, "deal_order_id", json_integer(deal_order_id));

            json_object_set_new(record, "market", json_string(row[10]));

            json_array_append_new(records, record);
        }
        mysql_free_result(result);

        return records;
    }
    // else SELECT FROM postgre
    sds base_sql = sdsempty();
    base_sql = sdscatprintf(base_sql, "SELECT extract(epoch FROM \"time\")  AS \"time\","
                                 "user_id AS user, deal_id AS id, side, \"role\", "
                                 "to_char(price,'%s') as price, "
                                 "to_char(amount,'%s') as amount, "
                                 "to_char(deal,'%s') as deal, "
                                 "to_char(fee,'%s') as fee, "
                                 "to_char(deal_fee,'%s') as deal_fee, "
                                 "deal_order_id, market"
                                 "FROM user_deal_history "
                                 "WHERE user_id = %u AND market = '%s' ORDER BY id DESC",
                            NUM_FMT, NUM_FMT, NUM_FMT, NUM_FMT, NUM_FMT, user_id, market);


    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT array_to_json( array_agg(row_to_json(t)) )  FROM ( %s ) t", base_sql);
    sdsfree(base_sql);

    log_trace("exec postgresql sql: %s", sql);
    PGresult* res = PQexec(conn->postgresql, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        sdsfree(sql);
        return NULL;
    }
    sdsfree(sql);
    size_t num_rows = PQntuples(res);
    if (num_rows == 0) {
        PQclear(res);
        return json_array();
    }
    const char* result_str = PQgetvalue(res, 0, 0);
    json_t *records = json_loads(result_str, 0, NULL);
    PQclear(res);
    if (records == NULL) {
        // 'dirty' but it's fast and no need change sql array_to_json( array_agg(row_to_json(t)) )
        // NOTE may be need change sql
        return json_array();
    }
    return records;
}

json_t *get_all_order_finished_to_buy(struct db_connection *conn, const char *market, uint64_t start_time, uint64_t end_time)
{
    if (is_select_from_mysql(conn)) {
        size_t market_len = strlen(market);
        char _market[2 * market_len + 1];
        mysql_real_escape_string(conn->mysql, _market, market, market_len);

        sds sql = sdsempty();
        for (int user_id = 0; user_id < 99; ++user_id) // until _98
        {
            sql = sdscatprintf(sql,
                               "SELECT `id`, `finish_time`, `price`, `amount` "
                               "FROM `order_history_%u` "
                               "WHERE `side` = 2 "        // take only buys (hardcode)
                               "AND `market` = '%s'", user_id % HISTORY_HASH_NUM, _market);
            if (start_time) {
                sql = sdscatprintf(sql, " AND `finish_time` >= %"PRIu64, start_time);
            }
            if (end_time) {
                sql = sdscatprintf(sql, " AND `finish_time` < %"PRIu64, end_time);
            }
            sql = sdscatprintf(sql, " UNION ");
        }

        sql = sdscatprintf(sql,
                           "SELECT `id`, `finish_time`, `price`, `amount` "
                           "FROM `order_history_99` "    // LAST _99
                           "WHERE `side` = 2 "            // take only buys (hardcode)
                           "AND `market` = '%s'", _market);
        if (start_time) {
            sql = sdscatprintf(sql, " AND `finish_time` >= %"PRIu64, start_time);
        }
        if (end_time) {
            sql = sdscatprintf(sql, " AND `finish_time` < %"PRIu64, end_time);
        }
        sql = sdscatprintf(sql, " ORDER BY `finish_time`");

        log_trace("exec sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return NULL;
        }
        sdsfree(sql);

        MYSQL_RES *result = mysql_store_result(conn->mysql);
        size_t num_rows = mysql_num_rows(result);
        json_t *records = json_array();
        for (size_t i = 0; i < num_rows; ++i) {
            json_t *record = json_object();
            MYSQL_ROW row = mysql_fetch_row(result);
            uint64_t order_id = strtoull(row[0], NULL, 0);
            json_object_set_new(record, "id", json_integer(order_id));
            double ftime = strtod(row[1], NULL);
            json_object_set_new(record, "ftime", json_real(ftime));
            json_object_set_new(record, "price", json_string(rstripzero(row[2])));
            json_object_set_new(record, "amount", json_string(rstripzero(row[3])));

            json_array_append_new(records, record);
        }
        mysql_free_result(result);
        return records;
    }
    // else SELECT FROM postgre
    sds base_sql = sdsempty();
    base_sql = sdscatprintf(base_sql, "SELECT id, extract(epoch FROM finish_time) AS ftime,"
                                "       to_char(price,'%s') as price,"
                                "       to_char(amount,'%s') as amount "
                                "FROM order_history "
                                "WHERE side = 2 AND market = '%s'", NUM_FMT, NUM_FMT, market);
    if (start_time) {
        base_sql = sdscatprintf(base_sql, " AND finish_time >= to_timestamp(%"PRIu64")", start_time);
    }
    if (end_time) {
        base_sql = sdscatprintf(base_sql, " AND finish_time < to_timestamp(%"PRIu64")", end_time);
    }
    base_sql = sdscatprintf(base_sql, " ORDER BY finish_time");

    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT array_to_json( array_agg(row_to_json(t)) ) FROM ( %s ) t", base_sql);
    sdsfree(base_sql);

    log_trace("exec postgresql sql: %s", sql);
    PGresult* res = PQexec(conn->postgresql, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        sdsfree(sql);
        return NULL;
    }
    sdsfree(sql);
    size_t num_rows = PQntuples(res);
    if (num_rows != 1) {
        PQclear(res);
        return json_array();
    }
    const char* result_str = PQgetvalue(res, 0, 0);
    json_t *records = json_loads(result_str, 0, NULL);
    PQclear(res);
    if (records == NULL) {
        // 'dirty' but it's fast and no need change sql array_to_json( array_agg(row_to_json(t)) )
        // NOTE may be need change sql
        return json_array();
    }
    return records;
}

json_t *get_asset_status(struct db_connection *conn, uint64_t start_time, uint64_t end_time)
{
    if (is_select_from_mysql(conn)) {
        sds sql = sdsempty();
        sql = sdscatprintf(sql, "SELECT `asset`, "
                                "SUM(IF(`business` = 'fee', `change`, 0)) AS `fee_amount`, "
                                "COUNT(IF(`business` = 'fee', `change`, null)) AS `fee_count`, "
                                "SUM(IF(`business` = 'deposit', `change`, 0)) AS `deposit_amount`, "
                                "COUNT(IF(`business` = 'deposit', `change`, null)) AS `deposit_count`, "
                                "SUM(IF(`business` = 'withdrawal', `change`, 0)) AS `withdrawal_amount`, "
                                "COUNT(IF(`business` = 'withdrawal', `change`, null)) AS `withdrawal_count` "
                                "FROM trade_history.exchange_data");
        if (start_time) {
            sql = sdscatprintf(sql, " WHERE `time` >= %"PRIu64, start_time);
        }
        if (end_time) {
            sql = sdscatprintf(sql, " AND `time` < %"PRIu64, end_time);
        }
        sql = sdscatprintf(sql, " GROUP BY `asset`");

        log_trace("exec sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return NULL;
        }
        sdsfree(sql);

        MYSQL_RES *result = mysql_store_result(conn->mysql);
        size_t num_rows = mysql_num_rows(result);

        json_t *records = json_array();
        for (size_t i = 0; i < num_rows; ++i) {
            json_t *record = json_object();
            MYSQL_ROW row = mysql_fetch_row(result);

            uint64_t fee_count = strtoull(row[2], NULL, 0);
            uint64_t deposit_count = strtoull(row[4], NULL, 0);
            uint64_t withdrawal_count = strtoull(row[6], NULL, 0);
            json_object_set_new(record, "name", json_string(rstripzero(row[0])));
            json_object_set_new(record, "fee_amount", json_string(rstripzero(row[1])));
            json_object_set_new(record, "fee_count", json_integer(fee_count));
            json_object_set_new(record, "deposit_amount", json_string(rstripzero(row[3])));
            json_object_set_new(record, "deposit_count", json_integer(deposit_count));
            json_object_set_new(record, "withdrawal_amount", json_string(rstripzero(row[5])));
            json_object_set_new(record, "withdrawal_count", json_integer(withdrawal_count));

            json_array_append_new(records, record);
        }

        mysql_free_result(result);
        return records;
    }
    // else SELECT FROM postgre
    sds base_sql = sdsempty();
    base_sql = sdscatprintf(base_sql, "SELECT asset AS name, "
                              "to_char(SUM(CASE WHEN(operation = 'fee') then amount else 0 end),'%s') AS fee_amount, "
                              "COUNT(CASE WHEN(operation = 'fee') then amount else null end) AS fee_count, "
                              "to_char(SUM(CASE WHEN(operation = 'deposit') then amount else 0 end),'%s') AS deposit_amount, "
                              "COUNT(CASE WHEN(operation = 'deposit') then amount else null end) AS deposit_count, "
                              "to_char(SUM(CASE WHEN(operation = 'withdrawal') then amount else 0 end),'%s') AS withdrawal_amount, "
                              "COUNT(CASE WHEN(operation = 'withdrawal') then amount else null end) AS withdrawal_count "
                              "FROM exchange_data WHERE 1=1", NUM_FMT, NUM_FMT, NUM_FMT);
    if (start_time) {
        base_sql = sdscatprintf(base_sql, " AND finish_time >= to_timestamp(%"PRIu64")", start_time);
    }
    if (end_time) {
        base_sql = sdscatprintf(base_sql, " AND finish_time < to_timestamp(%"PRIu64")", end_time);
    }
    base_sql = sdscatprintf(base_sql, " GROUP BY asset");

    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT array_to_json( array_agg(row_to_json(t)) ) FROM ( %s ) t", base_sql);
    sdsfree(base_sql);

    log_trace("exec postgresql sql: %s", sql);
    PGresult* res = PQexec(conn->postgresql, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        sdsfree(sql);
        return NULL;
    }
    sdsfree(sql);
    size_t num_rows = PQntuples(res);
    if (num_rows != 1) {
        PQclear(res);
        return json_array();
    }
    const char* result_str = PQgetvalue(res, 0, 0);
    json_t *records = json_loads(result_str, 0, NULL);
    PQclear(res);
    if (records == NULL) {
        // 'dirty' but it's fast and no need change sql array_to_json( array_agg(row_to_json(t)) )
        // NOTE may be need change sql
        return json_array();
    }
    return records;
}

json_t *calculation_finished_orders(struct db_connection *conn, uint64_t start_time, uint64_t end_time)
{
    if (is_select_from_mysql(conn)) {
        sds base_sql = sdsempty();
        for (int user_id = 0; user_id < 99; ++user_id) // until _98
        {
            base_sql = sdscatprintf(base_sql,
                               "SELECT `market`, COUNT(1) as count "
                               "FROM `order_history_%u` "
                               "WHERE `side` = 2 ",      // take only buys (hardcode)
                                user_id % HISTORY_HASH_NUM);
            if (start_time) {
                base_sql = sdscatprintf(base_sql, " AND `finish_time` >= %"PRIu64, start_time);
            }
            if (end_time) {
                base_sql = sdscatprintf(base_sql, " AND `finish_time` < %"PRIu64, end_time);
            }
            base_sql = sdscat(base_sql, " GROUP BY `market` UNION ALL ");
        }
        // LAST _99
        // take only buys (hardcode)
        base_sql = sdscat(base_sql, "SELECT `market`, COUNT(1) as count FROM `order_history_99` WHERE `side` = 2 ");
        if (start_time) {
            base_sql = sdscatprintf(base_sql, " AND `finish_time` >= %"PRIu64, start_time);
        }
        if (end_time) {
            base_sql = sdscatprintf(base_sql, " AND `finish_time` < %"PRIu64, end_time);
        }
        base_sql = sdscat(base_sql, " GROUP BY `market` ");

        sds sql = sdsempty();
        sql = sdscatprintf(sql, "SELECT `market`, SUM(t.count) FROM ( %s ) t GROUP BY `market` ORDER BY 2 DESC;", base_sql);
        sdsfree(base_sql);
        log_trace("exec sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return NULL;
        }
        sdsfree(sql);

        MYSQL_RES *result = mysql_store_result(conn->mysql);
        size_t num_rows = mysql_num_rows(result);
        json_t *records = json_array();
        for (size_t i = 0; i < num_rows; ++i) {
            json_t *record = json_object();
            MYSQL_ROW row = mysql_fetch_row(result);

            json_object_set_new(record, "market", json_string(row[0]));
            uint64_t count = strtoull(row[1], NULL, 0);
            json_object_set_new(record, "count", json_integer(count));
            json_array_append_new(records, record);
        }

        mysql_free_result(result);
        return records;
    }
    // else SELECT FROM postgre
    sds base_sql = sdsempty();
    base_sql = sdscat(base_sql, "SELECT market, COUNT(id) FROM order_history  WHERE side = 2 ");
    if (start_time) {
        base_sql = sdscatprintf(base_sql, " AND finish_time >= to_timestamp(%"PRIu64")", start_time);
    }
    if (end_time) {
        base_sql = sdscatprintf(base_sql, " AND finish_time < to_timestamp(%"PRIu64")", end_time);
    }
    base_sql = sdscat(base_sql, " GROUP BY market ORDER BY 2 DESC ");

    sds sql = sdsempty();
    sql = sdscatprintf(sql, "SELECT array_to_json( array_agg(row_to_json(t)) ) FROM ( %s ) t", base_sql);
    sdsfree(base_sql);
    log_trace("exec postgresql sql: %s", sql);
    PGresult* res = PQexec(conn->postgresql, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        sdsfree(sql);
        return NULL;
    }
    sdsfree(sql);
    size_t num_rows = PQntuples(res);
    if (num_rows != 1) {
        PQclear(res);
        return json_array();
    }
    const char* result_str = PQgetvalue(res, 0, 0);
    json_t *records = json_loads(result_str, 0, NULL);
    PQclear(res);
    if (records == NULL) {
        // 'dirty' but it's fast and no need change sql array_to_json( array_agg(row_to_json(t)) )
        // NOTE may be need change sql
        return json_array();
    }
    return records;
}

