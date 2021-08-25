/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/04, create
 */

# include "ut_mysql.h"
# include "me_trade.h"
# include "me_market.h"
# include "me_balance.h"

static sds sql_append_mpd(sds sql, mpd_t *val, bool comma)
{
    char *str = mpd_to_sci(val, 0);
    sql = sdscatprintf(sql, "'%s'", str);
    if (comma) {
        sql = sdscatprintf(sql, ", ");
    }
    free(str);
    return sql;
}

sds get_mysql_order_insert_str(sds sql, const char *table, order_t *order) {
    if (sql && sdslen(sql) > 0) {
        sql = sdscatprintf(sql, ", ");
    } else {
        sql = sdscatprintf(sql, "INSERT INTO `%s` (`id`, `t`, `side`, `create_time`, `update_time`, `user_id`, `market`, "
                                "`price`, `amount`, `taker_fee`, `maker_fee`, `left`, `freeze`, `deal_stock`, `deal_money`, `deal_fee`) VALUES ", table);
    }
    sql = sdscatprintf(sql, "(%"PRIu64", %u, %u, %f, %f, %u, '%s' , ",
                       order->id, order->type, order->side, order->create_time, order->update_time, order->user_id, order->market);
    sql = sql_append_mpd(sql, order->price, true);
    sql = sql_append_mpd(sql, order->amount, true);
    sql = sql_append_mpd(sql, order->taker_fee, true);
    sql = sql_append_mpd(sql, order->maker_fee, true);
    sql = sql_append_mpd(sql, order->left, true);
    sql = sql_append_mpd(sql, order->freeze, true);
    sql = sql_append_mpd(sql, order->deal_stock, true);
    sql = sql_append_mpd(sql, order->deal_money, true);
    sql = sql_append_mpd(sql, order->deal_fee, false);
    sql = sdscatprintf(sql, ")");
    return sql;
}

sds get_postgresql_order_insert_str(sds sql, const char *table, order_t *order) {
    if (sql && sdslen(sql) > 0) {
        sql = sdscatprintf(sql, ", ");
    } else {
        sql = sdscatprintf(sql, "INSERT INTO \"%s\" (id, order_type, side, create_time, update_time,"
                                " user_id, market, price, amount, taker_fee, maker_fee, \"left\", \"freeze\", deal_stock,"
                                " deal_money, deal_fee) VALUES ", table);
    }
    sql = sdscatprintf(sql, "(%"PRIu64", %u, %u, to_timestamp(%f), to_timestamp(%f), %u, '%s' , ",
                       order->id, order->type, order->side, order->create_time, order->update_time, order->user_id, order->market);
    sql = sql_append_mpd(sql, order->price, true);
    sql = sql_append_mpd(sql, order->amount, true);
    sql = sql_append_mpd(sql, order->taker_fee, true);
    sql = sql_append_mpd(sql, order->maker_fee, true);
    sql = sql_append_mpd(sql, order->left, true);
    sql = sql_append_mpd(sql, order->freeze, true);
    sql = sql_append_mpd(sql, order->deal_stock, true);
    sql = sql_append_mpd(sql, order->deal_money, true);
    sql = sql_append_mpd(sql, order->deal_fee, false);
    sql = sdscatprintf(sql, ")");
    return sql;
}

static int dump_orders_list(struct db_connection *conn, const char *table, skiplist_t *list)
{
    sds sql_mysql = sdsempty();
    sds sql_postgresql = sdsempty();
    size_t insert_limit = 1000;
    size_t index = 0;
    skiplist_iter *iter = skiplist_get_iterator(list);
    skiplist_node *node;
    while ((node = skiplist_next(iter)) != NULL) {
        order_t *order = node->value;
        if (is_need_insert_my(conn))
            sql_mysql = get_mysql_order_insert_str(sql_mysql, table, order);
        if (is_need_insert_pg(conn))
            sql_postgresql = get_postgresql_order_insert_str(sql_postgresql, table, order);
        index += 1;
        if (index == insert_limit) {

            if (is_need_insert_my(conn)) {
                log_trace("exec mysql sql: %s", sql_mysql);

                int ret = mysql_real_query(conn->mysql, sql_mysql, sdslen(sql_mysql));
                if (ret < 0) {
                    log_error("exec sql: %s fail: %d %s", sql_mysql, mysql_errno(conn->mysql),
                              mysql_error(conn->mysql));
                    skiplist_release_iterator(iter);
                    sdsfree(sql_mysql);
                    sdsfree(sql_postgresql);
                    return -__LINE__;
                }
                sdsclear(sql_mysql);
            }

            if (is_need_insert_pg(conn)) {
                log_trace("exec postgresql sql: %s", sql_postgresql);
                PGresult *res;
                res = PQexec(conn->postgresql, sql_postgresql);
                if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
                    log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
                    skiplist_release_iterator(iter);
                    PQclear(res);
                    sdsfree(sql_mysql);
                    sdsfree(sql_postgresql);
                    return -__LINE__;
                }

                PQclear(res);
                sdsclear(sql_postgresql);
            }
            index = 0;
        }
    }
    skiplist_release_iterator(iter);

    if (index > 0) {
        if (is_need_insert_my(conn)) {
            log_trace("exec mysql sql: %s", sql_mysql);
            int ret = mysql_real_query(conn->mysql, sql_mysql, sdslen(sql_mysql));
            if (ret < 0) {
                log_error("exec sql: %s fail: %d %s", sql_mysql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
                sdsfree(sql_mysql);
                sdsfree(sql_postgresql);
                return -__LINE__;
            }
        }
        if (is_need_insert_pg(conn)) {
            log_trace("exec postgresql sql: %s", sql_postgresql);
            PGresult *res;
            res = PQexec(conn->postgresql, sql_postgresql);
            if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
                log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
                PQclear(res);
                sdsfree(sql_mysql);
                sdsfree(sql_postgresql);
                return -__LINE__;
            }

            PQclear(res);
            sdsclear(sql_postgresql);
        }

    }

    sdsfree(sql_mysql);
    sdsfree(sql_postgresql);
    return 0;
}

int dump_orders(struct db_connection *conn, const char *table)
{
    if (is_need_insert_my(conn)) {
        sds sql = sdsempty();
        sql = sdscatprintf(sql, "DROP TABLE IF EXISTS `%s`", table);
        log_trace("exec sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return -__LINE__;
        }
        sdsclear(sql);

        sql = sdscatprintf(sql, "CREATE TABLE IF NOT EXISTS `%s` LIKE `slice_order_example`", table);
        log_trace("exec sql: %s", sql);
        ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return -__LINE__;
        }
        sdsfree(sql);
    }
    if (is_need_insert_pg(conn)) {
        sds sql = sdsempty();
        sql = sdscatprintf(sql, "CREATE TABLE IF NOT EXISTS \"%s\" (LIKE slice_order)", table);
        PGresult *res;
        res = PQexec(conn->postgresql, sql);
        if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
            log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        }
        PQclear(res);
        sdsfree(sql);
    }

    for (int i = 0; i < settings.market_num; ++i) {
        market_t *market = get_market(settings.markets[i].name);
        if (market == NULL) {
            return -__LINE__;
        }
        int ret;
        ret = dump_orders_list(conn, table, market->asks);
        if (ret < 0) {
            log_error("dump market: %s asks orders list fail: %d", market->name, ret);
            return -__LINE__;
        }
        ret = dump_orders_list(conn, table, market->bids);
        if (ret < 0) {
            log_error("dump market: %s bids orders list fail: %d", market->name, ret);
            return -__LINE__;
        }
    }

    return 0;
}

sds get_mysql_balance_insert_str(sds sql, const char *table, struct balance_key *key, mpd_t *balance) {
    if (sql && sdslen(sql) > 0) {
        sql = sdscatprintf(sql, ", ");
    } else {
        sql = sdscatprintf(sql, "INSERT INTO `%s` (`id`, `user_id`, `asset`, `t`, `balance`) VALUES ", table);
    }
    sql = sdscatprintf(sql, "(NULL, %u, '%s', %u, ", key->user_id, key->asset, key->type);
    sql = sql_append_mpd(sql, balance, false);
    sql = sdscatprintf(sql, ")");
    return sql;
}

sds get_postgresql_balance_insert_str(sds sql, const char *table, struct balance_key *key, mpd_t *balance) {
    if (sql && sdslen(sql) > 0) {
        sql = sdscatprintf(sql, ", ");
    } else {
        sql = sdscatprintf(sql, "INSERT INTO \"%s\" (user_id, asset, balance_type, balance) VALUES ", table);
    }
    sql = sdscatprintf(sql, "(%u, '%s', %u, ", key->user_id, key->asset, key->type);
    sql = sql_append_mpd(sql, balance, false);
    sql = sdscatprintf(sql, ")");
    return sql;
}

static int dump_balance_dict(struct db_connection *conn, const char *table, dict_t *dict)
{
    sds sql_mysql = sdsempty();
    sds sql_postgresql = sdsempty();

    size_t insert_limit = 1000;
    size_t index = 0;
    dict_iterator *iter = dict_get_iterator(dict);
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        struct balance_key *key = entry->key;
        mpd_t *balance = entry->val;
        if (is_need_insert_my(conn))
            sql_mysql = get_mysql_balance_insert_str(sql_mysql, table, key, balance);
        if (is_need_insert_pg(conn))
            sql_postgresql = get_postgresql_balance_insert_str(sql_postgresql, table, key, balance);

        index += 1;
        if (index == insert_limit) {
            if (is_need_insert_my(conn)) {
                log_trace("exec mysql sql: %s", sql_mysql);
                int ret = mysql_real_query(conn->mysql, sql_mysql, sdslen(sql_mysql));
                if (ret < 0) {
                    log_error("exec sql: %s fail: %d %s", sql_mysql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
                    dict_release_iterator(iter);
                    sdsfree(sql_mysql);
                    sdsfree(sql_postgresql);
                    return -__LINE__;
                }
                sdsclear(sql_mysql);
            }
            if (is_need_insert_pg(conn)) {
                log_trace("exec postgresql sql: %s", sql_postgresql);
                PGresult *res;
                res = PQexec(conn->postgresql, sql_postgresql);
                if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
                    log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
                    dict_release_iterator(iter);
                    PQclear(res);
                    sdsfree(sql_mysql);
                    sdsfree(sql_postgresql);
                    return -__LINE__;
                }

                PQclear(res);
                sdsclear(sql_postgresql);
            }
            index = 0;
        }
    }
    dict_release_iterator(iter);

    if (index > 0) {
        if (is_need_insert_my(conn)) {
            log_trace("exec sql: %s", sql_mysql);
            int ret = mysql_real_query(conn->mysql, sql_mysql, sdslen(sql_mysql));
            if (ret < 0) {
                log_error("exec sql: %s fail: %d %s", sql_mysql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
                sdsfree(sql_mysql);
                sdsfree(sql_postgresql);
                return -__LINE__;
            }
        }
        if (is_need_insert_pg(conn)) {
            log_trace("exec postgresql sql: %s", sql_postgresql);
            PGresult *res;
            res = PQexec(conn->postgresql, sql_postgresql);
            if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
                log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
                PQclear(res);
                sdsfree(sql_mysql);
                sdsfree(sql_postgresql);
                return -__LINE__;
            }

            PQclear(res);
            sdsclear(sql_postgresql);
        }
    }

    sdsfree(sql_mysql);
    sdsfree(sql_postgresql);
    return 0;
}

int dump_balance(struct db_connection *conn, const char *table)
{
    if (is_need_insert_my(conn)) {
        sds sql = sdsempty();
        sql = sdscatprintf(sql, "DROP TABLE IF EXISTS `%s`", table);
        log_trace("exec sql: %s", sql);
        int ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return -__LINE__;
        }
        sdsclear(sql);

        sql = sdscatprintf(sql, "CREATE TABLE IF NOT EXISTS `%s` LIKE `slice_balance_example`", table);
        log_trace("exec sql: %s", sql);
        ret = mysql_real_query(conn->mysql, sql, sdslen(sql));
        if (ret != 0) {
            log_error("exec sql: %s fail: %d %s", sql, mysql_errno(conn->mysql), mysql_error(conn->mysql));
            sdsfree(sql);
            return -__LINE__;
        }
        sdsfree(sql);
    }
    if (is_need_insert_pg(conn)) {
        sds sql = sdsempty();
        sql = sdscatprintf(sql, "CREATE TABLE IF NOT EXISTS \"%s\" (LIKE slice_balance); "
                                "ALTER TABLE \"%s\" ADD COLUMN id SERIAL PRIMARY KEY;", table, table);
        PGresult *res;
        res = PQexec(conn->postgresql, sql);
        if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
            log_fatal( "Error executing commands: %s\n", PQresultErrorMessage(res));
        }
        PQclear(res);
        sdsfree(sql);
    }


    int ret = dump_balance_dict(conn, table, dict_balance);
    if (ret < 0) {
        log_error("dump_balance_dict fail: %d", ret);
        return -__LINE__;
    }

    return 0;
}

