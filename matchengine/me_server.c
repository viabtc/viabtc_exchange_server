/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/16, create
 */

# include "me_config.h"
# include "me_server.h"
# include "me_balance.h"
# include "me_update.h"
# include "me_market.h"
# include "me_trade.h"
# include "me_operlog.h"
# include "me_history.h"
# include "me_message.h"

static rpc_svr *svr;

static int reply_json(nw_ses *ses, rpc_pkg *pkg, const json_t *json)
{
    char *message_data;
    if (settings.debug) {
        message_data = json_dumps(json, JSON_INDENT(4));
    } else {
        message_data = json_dumps(json, 0);
    }
    if (message_data == NULL)
        return -__LINE__;
    log_trace("connection: %s send: %s", nw_sock_human_addr(&ses->peer_addr), message_data);

    rpc_pkg reply;
    memcpy(&reply, pkg, sizeof(reply));
    reply.pkg_type = RPC_PKG_TYPE_REPLY;
    reply.body = message_data;
    reply.body_size = strlen(message_data);
    rpc_send(ses, &reply);
    free(message_data);

    return 0;
}

static int reply_error(nw_ses *ses, rpc_pkg *pkg, int code, const char *message)
{
    json_t *error = json_object();
    json_object_set_new(error, "code", json_integer(code));
    json_object_set_new(error, "message", json_string(message));

    json_t *reply = json_object();
    json_object_set_new(reply, "error", error);
    json_object_set_new(reply, "result", json_null());
    json_object_set_new(reply, "id", json_integer(pkg->req_id));

    int ret = reply_json(ses, pkg, reply);
    json_decref(reply);

    return ret;
}

static int reply_error_invalid_argument(nw_ses *ses, rpc_pkg *pkg)
{
    return reply_error(ses, pkg, 1, "invalid argument");
}

static int reply_error_internal_error(nw_ses *ses, rpc_pkg *pkg)
{
    return reply_error(ses, pkg, 2, "internal error");
}

static int reply_error_service_unavailable(nw_ses *ses, rpc_pkg *pkg)
{
    return reply_error(ses, pkg, 3, "service unavailable");
}

static int reply_result(nw_ses *ses, rpc_pkg *pkg, json_t *result)
{
    json_t *reply = json_object();
    json_object_set_new(reply, "error", json_null());
    json_object_set_new(reply, "result", result);
    json_object_set_new(reply, "id", json_integer(pkg->req_id));

    int ret = reply_json(ses, pkg, reply);
    json_decref(reply);

    return ret;
}

static int reply_success(nw_ses *ses, rpc_pkg *pkg)
{
    json_t *result = json_object();
    json_object_set_new(result, "status", json_string("success"));
    return reply_result(ses, pkg, result);
}

static int on_cmd_balance_query(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    size_t request_size = json_array_size(params);
    if (request_size == 0)
        return reply_error_invalid_argument(ses, pkg);

    if (!json_is_integer(json_array_get(params, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(params, 0));
    if (user_id == 0)
        return reply_error_invalid_argument(ses, pkg);

    json_t *result = json_object();
    if (request_size == 1) {
        for (size_t i = 0; i < settings.asset_num; ++i) {
            const char *asset = settings.assets[i].name;
            json_t *unit = json_object();

            mpd_t *available = balance_get(user_id, BALANCE_TYPE_AVAILABLE, asset);
            if (available) {
                json_object_set_new_mpd(unit, "available", available);
            } else {
                json_object_set_new(unit, "available", json_string("0"));
            }

            mpd_t *freeze = balance_get(user_id, BALANCE_TYPE_FREEZE, asset);
            if (freeze) {
                json_object_set_new_mpd(unit, "freeze", freeze);
            } else {
                json_object_set_new(unit, "freeze", json_string("0"));
            }

            json_object_set_new(result, asset, unit);
        }
    } else {
        for (size_t i = 1; i < request_size; ++i) {
            const char *asset = json_string_value(json_array_get(params, i));
            if (!asset || !asset_exist(asset)) {
                json_decref(result);
                return reply_error_invalid_argument(ses, pkg);
            }
            json_t *unit = json_object();

            mpd_t *available = balance_get(user_id, BALANCE_TYPE_AVAILABLE, asset);
            if (available) {
                json_object_set_new_mpd(unit, "available", available);
            } else {
                json_object_set_new(unit, "available", json_string("0"));
            }

            mpd_t *freeze = balance_get(user_id, BALANCE_TYPE_FREEZE, asset);
            if (freeze) {
                json_object_set_new_mpd(unit, "freeze", freeze);
            } else {
                json_object_set_new(unit, "freeze", json_string("0"));
            }

            json_object_set_new(result, asset, unit);
        }
    }

    return reply_result(ses, pkg, result);
}

static int on_cmd_balance_update(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 6)
        return reply_error_invalid_argument(ses, pkg);

    // user_id
    if (!json_is_integer(json_array_get(params, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(params, 0));

    // asset
    if (!json_is_string(json_array_get(params, 1)))
        return reply_error_invalid_argument(ses, pkg);
    const char *asset = json_string_value(json_array_get(params, 1));
    int prec = asset_prec(asset);
    if (prec < 0)
        return reply_error_invalid_argument(ses, pkg);

    // business
    if (!json_is_string(json_array_get(params, 2)))
        return reply_error_invalid_argument(ses, pkg);
    const char *business = json_string_value(json_array_get(params, 2));

    // business_id
    if (!json_is_integer(json_array_get(params, 3)))
        return reply_error_invalid_argument(ses, pkg);
    uint64_t business_id = json_integer_value(json_array_get(params, 3));

    // change
    if (!json_is_string(json_array_get(params, 4)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *change = decimal(json_string_value(json_array_get(params, 4)), prec);
    if (change == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // detail
    json_t *detail = json_array_get(params, 5);
    if (!json_is_object(detail))
        return reply_error_invalid_argument(ses, pkg);

    int ret = update_user_balance(true, user_id, asset, business, business_id, change, detail);
    mpd_del(change);
    if (ret == -1) {
        return reply_error(ses, pkg, 10, "repeat update");
    } else if (ret == -2) {
        return reply_error(ses, pkg, 11, "balance not enough");
    } else if (ret < 0) {
        return reply_error_internal_error(ses, pkg);
    }

    append_operlog("update_balance", params);
    return reply_success(ses, pkg);
}

static int on_cmd_order_put_limit(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 7)
        return reply_error_invalid_argument(ses, pkg);

    // user_id
    if (!json_is_integer(json_array_get(params, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(params, 0));

    // market
    if (!json_is_string(json_array_get(params, 1)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(params, 1));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // side
    if (!json_is_integer(json_array_get(params, 2)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t side = json_integer_value(json_array_get(params, 2));
    if (side != MARKET_ORDER_SIDE_ASK && side != MARKET_ORDER_SIDE_BID)
        return reply_error_invalid_argument(ses, pkg);

    // amount
    if (!json_is_string(json_array_get(params, 3)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *amount = decimal(json_string_value(json_array_get(params, 3)), market->stock_prec);
    if (amount == NULL)
        return reply_error_invalid_argument(ses, pkg);
    if (mpd_cmp(amount, mpd_zero, &mpd_ctx) <= 0) {
        mpd_del(amount);
        return reply_error_invalid_argument(ses, pkg);
    }

    // price 
    if (!json_is_string(json_array_get(params, 4)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *price = decimal(json_string_value(json_array_get(params, 4)), market->money_prec);
    if (price == NULL) {
        mpd_del(amount);
        return reply_error_invalid_argument(ses, pkg);
    }
    if (mpd_cmp(price, mpd_zero, &mpd_ctx) <= 0) {
        mpd_del(amount);
        mpd_del(price);
        return reply_error_invalid_argument(ses, pkg);
    }

    // taker fee
    if (!json_is_string(json_array_get(params, 5)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *taker_fee = decimal(json_string_value(json_array_get(params, 5)), market->fee_prec);
    if (taker_fee == NULL) {
        mpd_del(amount);
        mpd_del(price);
        return reply_error_invalid_argument(ses, pkg);
    }
    if (mpd_cmp(taker_fee, mpd_zero, &mpd_ctx) < 0 || mpd_cmp(taker_fee, mpd_one, &mpd_ctx) >= 0) {
        mpd_del(amount);
        mpd_del(price);
        mpd_del(taker_fee);
        return reply_error_invalid_argument(ses, pkg);
    }

    // maker fee
    if (!json_is_string(json_array_get(params, 6)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *maker_fee = decimal(json_string_value(json_array_get(params, 6)), market->fee_prec);
    if (maker_fee == NULL) {
        mpd_del(amount);
        mpd_del(price);
        mpd_del(taker_fee);
        return reply_error_invalid_argument(ses, pkg);
    }
    if (mpd_cmp(maker_fee, mpd_zero, &mpd_ctx) < 0 || mpd_cmp(maker_fee, mpd_one, &mpd_ctx) >= 0) {
        mpd_del(amount);
        mpd_del(price);
        mpd_del(taker_fee);
        mpd_del(maker_fee);
        return reply_error_invalid_argument(ses, pkg);
    }

    int ret = market_put_limit_order(true, market, user_id, side, amount, price, taker_fee, maker_fee);
    mpd_del(amount);
    mpd_del(price);
    mpd_del(taker_fee);
    mpd_del(maker_fee);
    if (ret == -1) {
        return reply_error(ses, pkg, 10, "balance not enough");
    } else if (ret < 0) {
        log_error("market_put_limit_order fail: %d", ret);
        return reply_error_internal_error(ses, pkg);
    }

    append_operlog("limit_order", params);
    return reply_success(ses, pkg);
}

static int on_cmd_order_put_market(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 5)
        return reply_error_invalid_argument(ses, pkg);

    // user_id
    if (!json_is_integer(json_array_get(params, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(params, 0));

    // market
    if (!json_is_string(json_array_get(params, 1)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(params, 1));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // side
    if (!json_is_integer(json_array_get(params, 2)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t side = json_integer_value(json_array_get(params, 2));
    if (side != MARKET_ORDER_SIDE_ASK && side != MARKET_ORDER_SIDE_BID)
        return reply_error_invalid_argument(ses, pkg);

    // amount
    if (!json_is_string(json_array_get(params, 3)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *amount = decimal(json_string_value(json_array_get(params, 3)), market->stock_prec);
    if (amount == NULL)
        return reply_error_invalid_argument(ses, pkg);
    if (mpd_cmp(amount, mpd_zero, &mpd_ctx) <= 0) {
        mpd_del(amount);
        return reply_error_invalid_argument(ses, pkg);
    }

    // taker fee
    if (!json_is_string(json_array_get(params, 4)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *taker_fee = decimal(json_string_value(json_array_get(params, 4)), market->fee_prec);
    if (taker_fee == NULL) {
        mpd_del(amount);
        return reply_error_invalid_argument(ses, pkg);
    }
    if (mpd_cmp(taker_fee, mpd_zero, &mpd_ctx) < 0 || mpd_cmp(taker_fee, mpd_one, &mpd_ctx) >= 0) {
        mpd_del(amount);
        mpd_del(taker_fee);
        return reply_error_invalid_argument(ses, pkg);
    }

    int ret = market_put_market_order(true, market, user_id, side, amount, taker_fee);
    mpd_del(amount);
    mpd_del(taker_fee);
    if (ret == -1) {
        return reply_error(ses, pkg, 10, "balance not enough");
    } else if (ret < 0) {
        log_error("market_put_limit_order fail: %d", ret);
        return reply_error_internal_error(ses, pkg);
    }

    append_operlog("market_order", params);
    return reply_success(ses, pkg);
}

static int on_cmd_order_query(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 4)
        return reply_error_invalid_argument(ses, pkg);

    // user_id
    if (!json_is_integer(json_array_get(params, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(params, 0));

    // market
    if (!json_is_string(json_array_get(params, 1)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(params, 1));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // offset
    if (!json_is_integer(json_array_get(params, 2)))
        return reply_error_invalid_argument(ses, pkg);
    size_t offset = json_integer_value(json_array_get(params, 2));

    // limit
    if (!json_is_integer(json_array_get(params, 3)))
        return reply_error_invalid_argument(ses, pkg);
    size_t limit = json_integer_value(json_array_get(params, 3));
    if (limit > ORDER_LIST_MAX_LEN)
        return reply_error_invalid_argument(ses, pkg);

    json_t *result = json_object();
    json_object_set_new(result, "limit", json_integer(limit));
    json_object_set_new(result, "offset", json_integer(offset));

    json_t *orders = json_array();
    list_t *order_list = market_get_order_list(market, user_id);
    if (order_list == NULL) {
        json_object_set_new(result, "total", json_integer(0));
    } else {
        json_object_set_new(result, "total", json_integer(order_list->len));
        list_iter *iter = list_get_iterator(order_list, LIST_START_HEAD);
        list_node *node;
        for (size_t i = 0; i < offset; i++) {
            list_next(iter);
        }
        size_t index = 0;
        while ((node = list_next(iter)) != NULL && index < limit) {
            index++;
            order_t *order = node->value;
            json_array_append_new(orders, get_order_info(order));
        }
        list_release_iterator(iter);
    }

    json_object_set_new(result, "orders", orders);
    return reply_result(ses, pkg, result);
}

static int on_cmd_order_cancel(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 3)
        return reply_error_invalid_argument(ses, pkg);

    // user_id
    if (!json_is_integer(json_array_get(params, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(params, 0));

    // market
    if (!json_is_string(json_array_get(params, 1)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(params, 1));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // order_id
    if (!json_is_integer(json_array_get(params, 2)))
        return reply_error_invalid_argument(ses, pkg);
    uint64_t order_id = json_integer_value(json_array_get(params, 2));

    order_t *order = market_get_order(market, order_id);
    if (order == NULL) {
        return reply_error(ses, pkg, 10, "order not found");
    }
    if (order->user_id != user_id) {
        return reply_error(ses, pkg, 11, "user not match");
    }

    int ret = market_cancel_order(true, market, order);
    if (ret < 0) {
        log_error("cancel order: %"PRIu64" fail: %d", order->id, ret);
        return reply_error_internal_error(ses, pkg);
    }

    append_operlog("cancel_order", params);
    return reply_success(ses, pkg);
}

static int on_cmd_order_book(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 4)
        return reply_error_invalid_argument(ses, pkg);

    // market
    if (!json_is_string(json_array_get(params, 0)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(params, 0));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // side
    if (!json_is_integer(json_array_get(params, 1)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t side = json_integer_value(json_array_get(params, 1));
    if (side != MARKET_ORDER_SIDE_ASK && side != MARKET_ORDER_SIDE_BID)
        return reply_error_invalid_argument(ses, pkg);

    // offset
    if (!json_is_integer(json_array_get(params, 2)))
        return reply_error_invalid_argument(ses, pkg);
    size_t offset = json_integer_value(json_array_get(params, 2));

    // limit
    if (!json_is_integer(json_array_get(params, 3)))
        return reply_error_invalid_argument(ses, pkg);
    size_t limit = json_integer_value(json_array_get(params, 3));
    if (limit > ORDER_BOOK_MAX_LEN)
        return reply_error_invalid_argument(ses, pkg);

    json_t *result = json_object();
    json_object_set_new(result, "offset", json_integer(offset));
    json_object_set_new(result, "limit", json_integer(limit));

    skiplist_iter *iter;
    if (side == MARKET_ORDER_SIDE_ASK) {
        iter = skiplist_get_iterator(market->asks);
        json_object_set_new(result, "total", json_integer(market->asks->len));
    } else {
        iter = skiplist_get_iterator(market->bids);
        json_object_set_new(result, "total", json_integer(market->bids->len));
    }

    for (size_t i = 0; i < offset; i++) {
        skiplist_next(iter);
    }

    json_t *orders = json_array();
    size_t index = 0;
    skiplist_node *node;
    while ((node = skiplist_next(iter)) != NULL && index < limit) {
        index++;
        order_t *order = node->value;
        json_array_append_new(orders, get_order_info(order));
    }
    skiplist_release_iterator(iter);

    json_object_set_new(result, "orders", orders);
    return reply_result(ses, pkg, result);
}

static json_t *get_depth(market_t *market, size_t limit)
{
    mpd_t *price = mpd_new(&mpd_ctx);
    mpd_t *amount = mpd_new(&mpd_ctx);

    json_t *asks = json_array();
    skiplist_iter *iter = skiplist_get_iterator(market->asks);
    skiplist_node *node = skiplist_next(iter);
    size_t index = 0;
    while (node && index < limit) {
        index++;
        order_t *order = node->value;
        mpd_copy(price, order->price, &mpd_ctx);
        mpd_copy(amount, order->left, &mpd_ctx);
        while ((node = skiplist_next(iter)) != NULL) {
            order = node->value;
            if (mpd_cmp(price, order->price, &mpd_ctx) == 0) {
                mpd_add(amount, amount, order->left, &mpd_ctx);
            } else {
                break;
            }
        }
        json_t *info = json_array();
        json_array_append_new_mpd(info, price);
        json_array_append_new_mpd(info, amount);
        json_array_append_new(asks, info);
    }
    skiplist_release_iterator(iter);

    json_t *bids = json_array();
    iter = skiplist_get_iterator(market->bids);
    node = skiplist_next(iter);
    index = 0;
    while (node && index < limit) {
        index++;
        order_t *order = node->value;
        mpd_copy(price, order->price, &mpd_ctx);
        mpd_copy(amount, order->left, &mpd_ctx);
        while ((node = skiplist_next(iter)) != NULL) {
            order = node->value;
            if (mpd_cmp(price, order->price, &mpd_ctx) == 0) {
                mpd_add(amount, amount, order->left, &mpd_ctx);
            } else {
                break;
            }
        }
        json_t *info = json_array();
        json_array_append_new_mpd(info, price);
        json_array_append_new_mpd(info, amount);
        json_array_append_new(bids, info);
    }
    skiplist_release_iterator(iter);

    mpd_del(price);
    mpd_del(amount);

    json_t *result = json_object();
    json_object_set_new(result, "asks", asks);
    json_object_set_new(result, "bids", bids);

    return result;
}

static json_t *get_depth_merge(market_t* market, size_t limit, mpd_t *interval)
{
    mpd_t *q = mpd_new(&mpd_ctx);
    mpd_t *r = mpd_new(&mpd_ctx);
    mpd_t *price = mpd_new(&mpd_ctx);
    mpd_t *amount = mpd_new(&mpd_ctx);

    json_t *asks = json_array();
    skiplist_iter *iter = skiplist_get_iterator(market->asks);
    skiplist_node *node = skiplist_next(iter);
    size_t index = 0;
    while (node && index < limit) {
        index++;
        order_t *order = node->value;
        mpd_divmod(q, r, order->price, interval, &mpd_ctx);
        mpd_mul(price, q, interval, &mpd_ctx);
        if (mpd_cmp(r, mpd_zero, &mpd_ctx) != 0) {
            mpd_add(price, price, interval, &mpd_ctx);
        }
        mpd_copy(amount, order->left, &mpd_ctx);
        while ((node = skiplist_next(iter)) != NULL) {
            order = node->value;
            if (mpd_cmp(price, order->price, &mpd_ctx) >= 0) {
                mpd_add(amount, amount, order->left, &mpd_ctx);
            } else {
                break;
            }
        }
        json_t *info = json_array();
        json_array_append_new_mpd(info, price);
        json_array_append_new_mpd(info, amount);
        json_array_append_new(asks, info);
    }
    skiplist_release_iterator(iter);

    json_t *bids = json_array();
    iter = skiplist_get_iterator(market->bids);
    node = skiplist_next(iter);
    index = 0;
    while (node && index < limit) {
        index++;
        order_t *order = node->value;
        mpd_divmod(q, r, order->price, interval, &mpd_ctx);
        mpd_mul(price, q, interval, &mpd_ctx);
        mpd_copy(amount, order->left, &mpd_ctx);
        while ((node = skiplist_next(iter)) != NULL) {
            order = node->value;
            if (mpd_cmp(price, order->price, &mpd_ctx) <= 0) {
                mpd_add(amount, amount, order->left, &mpd_ctx);
            } else {
                break;
            }
        }

        json_t *info = json_array();
        json_array_append_new_mpd(info, price);
        json_array_append_new_mpd(info, amount);
        json_array_append_new(bids, info);
    }
    skiplist_release_iterator(iter);

    mpd_del(q);
    mpd_del(r);
    mpd_del(price);
    mpd_del(amount);

    json_t *result = json_object();
    json_object_set_new(result, "asks", asks);
    json_object_set_new(result, "bids", bids);

    return result;
}

static int on_cmd_order_book_depth(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 3)
        return reply_error_invalid_argument(ses, pkg);

    // market
    if (!json_is_string(json_array_get(params, 0)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(params, 0));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // limit
    if (!json_is_integer(json_array_get(params, 1)))
        return reply_error_invalid_argument(ses, pkg);
    size_t limit = json_integer_value(json_array_get(params, 1));
    if (limit > ORDER_BOOK_MAX_LEN)
        return reply_error_invalid_argument(ses, pkg);

    // interval
    if (!json_is_string(json_array_get(params, 2)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *interval = decimal(json_string_value(json_array_get(params, 2)), market->money_prec);
    if (!interval)
        return reply_error_invalid_argument(ses, pkg);
    if (mpd_cmp(interval, mpd_zero, &mpd_ctx) < 0) {
        mpd_del(interval);
        return reply_error_invalid_argument(ses, pkg);
    }

    json_t *result = NULL;
    if (mpd_cmp(interval, mpd_zero, &mpd_ctx) == 0) {
        result = get_depth(market, limit);
    } else {
        result = get_depth_merge(market, limit, interval);
    }
    mpd_del(interval);

    if (result == NULL)
        return reply_error_internal_error(ses, pkg);

    return reply_result(ses, pkg, result);
}

static int on_cmd_order_detail(nw_ses *ses, rpc_pkg *pkg, json_t *params)
{
    if (json_array_size(params) != 2)
        return reply_error_invalid_argument(ses, pkg);

    // market
    if (!json_is_string(json_array_get(params, 0)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(params, 0));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // order_id
    if (!json_is_integer(json_array_get(params, 2)))
        return reply_error_invalid_argument(ses, pkg);
    uint64_t order_id = json_integer_value(json_array_get(params, 2));

    order_t *order = market_get_order(market, order_id);
    if (order == NULL) {
        return reply_result(ses, pkg, json_null());
    }

    return reply_result(ses, pkg, get_order_info(order));
}

static void svr_on_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
    json_t *params = json_loadb(pkg->body, pkg->body_size, 0, NULL);
    if (params == NULL || !json_is_array(params)) {
        goto decode_error;
    }
    sds params_str = sdsnewlen(pkg->body, pkg->body_size);

    int ret;
    switch (pkg->command) {
    case CMD_BALANCE_QUERY:
        log_debug("from: %s cmd balance query, params: %s", nw_sock_human_addr(&ses->peer_addr), params_str);
        ret = on_cmd_balance_query(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_balance_query %s fail: %d", params_str, ret);
        }
        break;
    case CMD_BALANCE_UPDATE:
        if (is_operlog_block() || is_history_block() || is_message_block()) {
            reply_error_service_unavailable(ses, pkg);
            goto cleanup;
        }
        log_debug("from: %s cmd balance update, params: %s", nw_sock_human_addr(&ses->peer_addr), params_str);
        ret = on_cmd_balance_update(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_balance_update %s fail: %d", params_str, ret);
        }
        break;
    case CMD_ORDER_PUT_LIMIT:
        if (is_operlog_block() || is_history_block() || is_message_block()) {
            reply_error_service_unavailable(ses, pkg);
            goto cleanup;
        }
        log_debug("from: %s cmd order put limit, params: %s", nw_sock_human_addr(&ses->peer_addr), params_str);
        ret = on_cmd_order_put_limit(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_order_put_limit %s fail: %d", params_str, ret);
        }
        break;
    case CMD_ORDER_PUT_MARKET:
        if (is_operlog_block() || is_history_block() || is_message_block()) {
            reply_error_service_unavailable(ses, pkg);
            goto cleanup;
        }
        log_debug("from: %s cmd order put market, params: %s", nw_sock_human_addr(&ses->peer_addr), params_str);
        ret = on_cmd_order_put_market(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_order_put_market %s fail: %d", params_str, ret);
        }
        break;
    case CMD_ORDER_QUERY:
        log_debug("from: %s cmd order query, params: %s", nw_sock_human_addr(&ses->peer_addr), params_str);
        ret = on_cmd_order_query(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_order_query %s fail: %d", params_str, ret);
        }
        break;
    case CMD_ORDER_CANCEL:
        if (is_operlog_block() || is_history_block() || is_message_block()) {
            reply_error_service_unavailable(ses, pkg);
            goto cleanup;
        }
        log_debug("from: %s cmd order cancel, params: %s", nw_sock_human_addr(&ses->peer_addr), params_str);
        ret = on_cmd_order_cancel(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_order_cancel %s fail: %d", params_str, ret);
        }
        break;
    case CMD_ORDER_BOOK:
        log_debug("from: %s cmd order book, params: %s", nw_sock_human_addr(&ses->peer_addr), params_str);
        ret = on_cmd_order_book(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_order_book %s fail: %d", params_str, ret);
        }
        break;
    case CMD_ORDER_BOOK_DEPTH:
        log_debug("from: %s cmd order book depth: %s", nw_sock_human_addr(&ses->peer_addr), params_str);
        ret = on_cmd_order_book_depth(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_order_book_depth %s fail: %d", params_str, ret);
        }
        break;
    case CMD_ORDER_DETAIL:
        log_debug("from: %s cmd order detail, params: %s", nw_sock_human_addr(&ses->peer_addr), params_str);
        ret = on_cmd_order_detail(ses, pkg, params);
        if (ret < 0) {
            log_error("on_cmd_order_detail %s fail: %d", params_str, ret);
        }
        break;
    default:
        log_error("from: %s unknown command: %u", nw_sock_human_addr(&ses->peer_addr), pkg->command);
        break;
    }

cleanup:
    sdsfree(params_str);
    json_decref(params);
    return;

decode_error:
    if (params) {
        json_decref(params);
    }
    sds hex = hexdump(pkg->body, pkg->body_size);
    log_error("connection: %s, cmd: %u decode params fail, params data: \n%s", \
            nw_sock_human_addr(&ses->peer_addr), pkg->command, hex);
    sdsfree(hex);
    rpc_svr_close_clt(svr, ses);

    return;
}

static void svr_on_new_connection(nw_ses *ses)
{
    log_trace("new connection: %s", nw_sock_human_addr(&ses->peer_addr));
}

static void svr_on_connection_close(nw_ses *ses)
{
    log_trace("connection: %s close", nw_sock_human_addr(&ses->peer_addr));
}

int init_server(void)
{
    rpc_svr_type type;
    memset(&type, 0, sizeof(type));
    type.on_recv_pkg = svr_on_recv_pkg;
    type.on_new_connection = svr_on_new_connection;
    type.on_connection_close = svr_on_connection_close;

    svr = rpc_svr_create(&settings.svr, &type);
    if (svr == NULL)
        return -__LINE__;
    if (rpc_svr_start(svr) < 0)
        return -__LINE__;

    return 0;
}

