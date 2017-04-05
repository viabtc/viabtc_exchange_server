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
# include "me_log.h"

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
    return reply_error(ses, pkg, 1, "internal error");
}

static int reply_result(nw_ses *ses, rpc_pkg *pkg, json_t *result)
{
    json_t *reply = json_object();
    json_object_set_new(reply, "error", json_null());
    json_object_set_new(reply, "result", result);

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

static int on_cmd_balance_query(nw_ses *ses, rpc_pkg *pkg, json_t *request)
{
    size_t request_size = json_array_size(request);
    if (request_size == 0 || request_size > 2)
        return reply_error_invalid_argument(ses, pkg);

    if (!json_is_integer(json_array_get(request, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(request, 0));

    if (request_size == 2) {
        if (!json_is_string(json_array_get(request, 1)))
            return reply_error_invalid_argument(ses, pkg);
        const char *asset = json_string_value(json_array_get(request, 1));
        if (asset_exist(asset) < 0)
            return reply_error_invalid_argument(ses, pkg);

        json_t *unit = json_object();
        mpd_t *available = balance_get(user_id, BALANCE_TYPE_AVAILABLE, asset);
        json_object_set_new(unit, "available", json_string(available == NULL ? "0" : mpd_to_sci(available, 0)));
        mpd_t *freeze = balance_get(user_id, BALANCE_TYPE_FREEZE, asset);
        json_object_set_new(unit, "freeze", json_string(freeze == NULL ? "0" : mpd_to_sci(freeze, 0)));
        json_t *result = json_object();
        json_object_set_new(result, asset, unit);

        return reply_result(ses, pkg, result);
    }

    json_t *result = json_object();
    for (size_t i = 0; i < settings.asset_num; ++i) {
        const char *asset = settings.assets[i].name;
        json_t *unit = json_object();
        mpd_t *available = balance_get(user_id, BALANCE_TYPE_AVAILABLE, asset);
        json_object_set_new(unit, "available", json_string(available == NULL ? "0" : mpd_to_sci(available, 0)));
        mpd_t *freeze = balance_get(user_id, BALANCE_TYPE_FREEZE, asset);
        json_object_set_new(unit, "freeze", json_string(freeze == NULL ? "0" : mpd_to_sci(freeze, 0)));
        json_object_set_new(result, asset, unit);
    }

    return reply_result(ses, pkg, result);
}

static int on_cmd_balance_update(nw_ses *ses, rpc_pkg *pkg, json_t *request)
{
    if (json_array_size(request) != 6)
        return reply_error_invalid_argument(ses, pkg);

    // user_id
    if (!json_is_integer(json_array_get(request, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(request, 0));

    // type
    if (!json_is_integer(json_array_get(request, 1)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t type = json_integer_value(json_array_get(request, 1));
    if (type != BALANCE_TYPE_AVAILABLE && type != BALANCE_TYPE_FREEZE)
        return reply_error_invalid_argument(ses, pkg);

    // asset
    if (!json_is_string(json_array_get(request, 2)))
        return reply_error_invalid_argument(ses, pkg);
    const char *asset = json_string_value(json_array_get(request, 2));
    int prec = asset_prec(asset);
    if (prec < 0)
        return reply_error_invalid_argument(ses, pkg);

    // business
    if (!json_is_string(json_array_get(request, 3)))
        return reply_error_invalid_argument(ses, pkg);
    const char *business = json_string_value(json_array_get(request, 3));

    // business_id
    if (!json_is_integer(json_array_get(request, 4)))
        return reply_error_invalid_argument(ses, pkg);
    uint64_t business_id = json_integer_value(json_array_get(request, 4));

    // change
    if (!json_is_string(json_array_get(request, 5)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *change = decimal(json_string_value(json_array_get(request, 5)), prec);
    if (change == NULL)
        return reply_error_invalid_argument(ses, pkg);

    int ret = update_user_balance(user_id, type, asset, business, business_id, change);
    mpd_del(change);
    if (ret == -1) {
        return reply_error(ses, pkg, 10, "repeat update");
    } else if (ret == -2) {
        return reply_error(ses, pkg, 11, "balance not enough");
    } else if (ret < 0) {
        return reply_error_internal_error(ses, pkg);
    }

    append_oper_log("update_balance", request);
    return reply_success(ses, pkg);
}

static int on_cmd_order_put_limit(nw_ses *ses, rpc_pkg *pkg, json_t *request)
{
    if (json_array_size(request) != 6)
        return reply_error_invalid_argument(ses, pkg);

    // user_id
    if (!json_is_integer(json_array_get(request, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(request, 0));

    // market
    if (!json_is_string(json_array_get(request, 1)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(request, 1));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // side
    if (!json_is_integer(json_array_get(request, 2)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t side = json_integer_value(json_array_get(request, 2));
    if (side != MARKET_ORDER_SIDE_ASK && side != MARKET_ORDER_SIDE_BID)
        return reply_error_invalid_argument(ses, pkg);

    // amount
    if (!json_is_string(json_array_get(request, 3)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *amount = decimal(json_string_value(json_array_get(request, 3)), market->stock_prec);
    if (amount == NULL)
        return reply_error_invalid_argument(ses, pkg);
    if (mpd_cmp(amount, mpd_zero, &mpd_ctx) <= 0) {
        mpd_del(amount);
        return reply_error_invalid_argument(ses, pkg);
    }

    // price 
    if (!json_is_string(json_array_get(request, 4)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *price = decimal(json_string_value(json_array_get(request, 4)), market->money_prec);
    if (price == NULL) {
        mpd_del(amount);
        return reply_error_invalid_argument(ses, pkg);
    }
    if (mpd_cmp(price, mpd_zero, &mpd_ctx) <= 0) {
        mpd_del(amount);
        mpd_del(price);
        return reply_error_invalid_argument(ses, pkg);
    }

    // fee
    if (!json_is_string(json_array_get(request, 5)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *fee = decimal(json_string_value(json_array_get(request, 5)), market->fee_prec);
    if (fee == NULL) {
        mpd_del(amount);
        mpd_del(price);
        return reply_error_invalid_argument(ses, pkg);
    }
    if (mpd_cmp(fee, mpd_zero, &mpd_ctx) < 0 || mpd_cmp(fee, mpd_one, &mpd_ctx) >= 0) {
        mpd_del(amount);
        mpd_del(price);
        mpd_del(fee);
        return reply_error_invalid_argument(ses, pkg);
    }

    int ret = market_put_limit_order(market, user_id, side, amount, price, fee);
    mpd_del(amount);
    mpd_del(price);
    mpd_del(fee);
    if (ret == -1) {
        return reply_error(ses, pkg, 10, "balance not enough");
    } else if (ret < 0) {
        log_error("market_put_limit_order fail: %d", ret);
        return reply_error_internal_error(ses, pkg);
    }

    append_oper_log("limit_order", request);
    return reply_success(ses, pkg);
}

static int on_cmd_order_put_market(nw_ses *ses, rpc_pkg *pkg, json_t *request)
{
    if (json_array_size(request) != 5)
        return reply_error_invalid_argument(ses, pkg);

    // user_id
    if (!json_is_integer(json_array_get(request, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(request, 0));

    // market
    if (!json_is_string(json_array_get(request, 1)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(request, 1));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // side
    if (!json_is_integer(json_array_get(request, 2)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t side = json_integer_value(json_array_get(request, 2));
    if (side != MARKET_ORDER_SIDE_ASK && side != MARKET_ORDER_SIDE_BID)
        return reply_error_invalid_argument(ses, pkg);

    // amount
    if (!json_is_string(json_array_get(request, 3)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *amount = decimal(json_string_value(json_array_get(request, 3)), market->stock_prec);
    if (amount == NULL)
        return reply_error_invalid_argument(ses, pkg);
    if (mpd_cmp(amount, mpd_zero, &mpd_ctx) <= 0) {
        mpd_del(amount);
        return reply_error_invalid_argument(ses, pkg);
    }

    // fee
    if (!json_is_string(json_array_get(request, 4)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *fee = decimal(json_string_value(json_array_get(request, 4)), market->fee_prec);
    if (fee == NULL) {
        mpd_del(amount);
        return reply_error_invalid_argument(ses, pkg);
    }
    if (mpd_cmp(fee, mpd_zero, &mpd_ctx) < 0 || mpd_cmp(fee, mpd_one, &mpd_ctx) >= 0) {
        mpd_del(amount);
        mpd_del(fee);
        return reply_error_invalid_argument(ses, pkg);
    }

    int ret = market_put_market_order(market, user_id, side, amount, fee);
    mpd_del(amount);
    mpd_del(fee);
    if (ret == -1) {
        return reply_error(ses, pkg, 10, "balance not enough");
    } else if (ret < 0) {
        log_error("market_put_limit_order fail: %d", ret);
        return reply_error_internal_error(ses, pkg);
    }

    append_oper_log("market_order", request);
    return reply_success(ses, pkg);
}

static json_t *get_order_info(order_t *order)
{
    json_t *info = json_object();
    json_object_set_new(info, "id", json_integer(order->id));
    json_object_set_new(info, "type", json_integer(order->type));
    json_object_set_new(info, "side", json_integer(order->side));
    json_object_set_new(info, "user", json_integer(order->user_id));
    json_object_set_new(info, "create_time", json_real(order->create_time));
    json_object_set_new(info, "update_time", json_real(order->update_time));
    json_object_set_new(info, "market", json_string(order->market));
    json_object_set_new(info, "price", json_string(mpd_to_sci(order->price, 0)));
    json_object_set_new(info, "amount", json_string(mpd_to_sci(order->amount, 0)));
    json_object_set_new(info, "fee", json_string(mpd_to_sci(order->fee, 0)));
    json_object_set_new(info, "left", json_string(mpd_to_sci(order->left, 0)));
    json_object_set_new(info, "deal_stock", json_string(mpd_to_sci(order->deal_stock, 0)));
    json_object_set_new(info, "deal_money", json_string(mpd_to_sci(order->deal_money, 0)));
    json_object_set_new(info, "deal_fee", json_string(mpd_to_sci(order->deal_fee, 0)));

    return info;
}

static int on_cmd_order_query(nw_ses *ses, rpc_pkg *pkg, json_t *request)
{
    if (json_array_size(request) != 4)
        return reply_error_invalid_argument(ses, pkg);

    // user_id
    if (!json_is_integer(json_array_get(request, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(request, 0));

    // market
    if (!json_is_string(json_array_get(request, 1)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(request, 1));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // offset
    if (!json_is_integer(json_array_get(request, 2)))
        return reply_error_invalid_argument(ses, pkg);
    size_t offset = json_integer_value(json_array_get(request, 2));

    // limit
    if (!json_is_integer(json_array_get(request, 3)))
        return reply_error_invalid_argument(ses, pkg);
    size_t limit = json_integer_value(json_array_get(request, 3));
    if (limit > ORDER_LIST_MAX_LEN)
        return reply_error_invalid_argument(ses, pkg);

    json_t *result = json_object();
    list_t *order_list = market_get_order_list(market, user_id);
    json_object_set_new(result, "total", json_integer(order_list->len));
    json_object_set_new(result, "limit", json_integer(limit));
    json_object_set_new(result, "offset", json_integer(offset));
    json_t *orders = json_array();
    if (order_list == NULL) {
        json_object_set_new(result, "orders", orders);
        return reply_result(ses, pkg, result);
    }

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

    json_object_set_new(result, "orders", orders);
    return reply_result(ses, pkg, result);
}

static int on_cmd_order_cancel(nw_ses *ses, rpc_pkg *pkg, json_t *request)
{
    if (json_array_size(request) != 3)
        return reply_error_invalid_argument(ses, pkg);

    // user_id
    if (!json_is_integer(json_array_get(request, 0)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t user_id = json_integer_value(json_array_get(request, 0));

    // market
    if (!json_is_string(json_array_get(request, 1)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(request, 1));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // order_id
    if (!json_is_integer(json_array_get(request, 2)))
        return reply_error_invalid_argument(ses, pkg);
    uint64_t order_id = json_integer_value(json_array_get(request, 2));

    order_t *order = market_get_order(market, order_id);
    if (order == NULL) {
        return reply_error(ses, pkg, 10, "order not found");
    }
    if (order->user_id != user_id) {
        return reply_error(ses, pkg, 11, "user not match");
    }

    market_cancel_order(market, order);

    return reply_success(ses, pkg);
}

static int on_cmd_order_book(nw_ses *ses, rpc_pkg *pkg, json_t *request)
{
    if (json_array_size(request) != 4)
        return reply_error_invalid_argument(ses, pkg);

    // market
    if (!json_is_string(json_array_get(request, 0)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(request, 0));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // side
    if (!json_is_integer(json_array_get(request, 1)))
        return reply_error_invalid_argument(ses, pkg);
    uint32_t side = json_integer_value(json_array_get(request, 1));
    if (side != MARKET_ORDER_SIDE_ASK && side != MARKET_ORDER_SIDE_BID)
        return reply_error_invalid_argument(ses, pkg);

    // offset
    if (!json_is_integer(json_array_get(request, 2)))
        return reply_error_invalid_argument(ses, pkg);
    size_t offset = json_integer_value(json_array_get(request, 2));

    // limit
    if (!json_is_integer(json_array_get(request, 3)))
        return reply_error_invalid_argument(ses, pkg);
    size_t limit = json_integer_value(json_array_get(request, 3));
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

static int on_cmd_order_book_depth(nw_ses *ses, rpc_pkg *pkg, json_t *request)
{
    if (json_array_size(request) != 2)
        return reply_error_invalid_argument(ses, pkg);

    // market
    if (!json_is_string(json_array_get(request, 0)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(request, 0));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // limit
    if (!json_is_integer(json_array_get(request, 1)))
        return reply_error_invalid_argument(ses, pkg);
    size_t limit = json_integer_value(json_array_get(request, 1));
    if (limit > ORDER_BOOK_MAX_LEN)
        return reply_error_invalid_argument(ses, pkg);

    json_t *asks = json_array();
    skiplist_iter *iter = skiplist_get_iterator(market->asks);
    skiplist_node *node;
    size_t index = 0;
    while ((node = skiplist_next(iter)) != NULL && index < limit) {
        index++;
        order_t *order = node->value;
        json_t *info = json_array();
        json_array_append_new(info, json_string(mpd_to_sci(order->price, 0)));
        json_array_append_new(info, json_string(mpd_to_sci(order->left, 0)));
        json_array_append_new(asks, info);
    }
    skiplist_release_iterator(iter);

    json_t *bids = json_array();
    iter = skiplist_get_iterator(market->bids);
    index = 0;
    while ((node = skiplist_next(iter)) != NULL) {
        index++;
        order_t *order = node->value;
        json_t *info = json_array();
        json_array_append_new(info, json_string(mpd_to_sci(order->price, 0)));
        json_array_append_new(info, json_string(mpd_to_sci(order->left, 0)));
        json_array_append_new(bids, info);
    }
    skiplist_release_iterator(iter);

    json_t *result = json_object();
    json_object_set_new(result, "asks", asks);
    json_object_set_new(result, "bids", bids);

    return reply_result(ses, pkg, result);
}

static int on_cmd_order_book_merge(nw_ses *ses, rpc_pkg *pkg, json_t *request)
{
    if (json_array_size(request) != 3)
        return reply_error_invalid_argument(ses, pkg);

    // market
    if (!json_is_string(json_array_get(request, 0)))
        return reply_error_invalid_argument(ses, pkg);
    const char *market_name = json_string_value(json_array_get(request, 0));
    market_t *market = get_market(market_name);
    if (market == NULL)
        return reply_error_invalid_argument(ses, pkg);

    // limit
    if (!json_is_integer(json_array_get(request, 1)))
        return reply_error_invalid_argument(ses, pkg);
    size_t limit = json_integer_value(json_array_get(request, 1));
    if (limit > ORDER_BOOK_MAX_LEN)
        return reply_error_invalid_argument(ses, pkg);

    // interval
    if (!json_is_string(json_array_get(request, 2)))
        return reply_error_invalid_argument(ses, pkg);
    mpd_t *interval = decimal(json_string_value(json_array_get(request, 2)), market->money_prec);
    if (!interval)
        return reply_error_invalid_argument(ses, pkg);
    if (mpd_cmp(interval, mpd_zero, &mpd_ctx) < 0) {
        mpd_del(interval);
        return reply_error_invalid_argument(ses, pkg);
    }

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
        json_array_append_new(info, json_string(mpd_to_sci(price, 0)));
        json_array_append_new(info, json_string(mpd_to_sci(amount, 0)));
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
        json_array_append_new(info, json_string(mpd_to_sci(price, 0)));
        json_array_append_new(info, json_string(mpd_to_sci(amount, 0)));
        json_array_append_new(bids, info);
    }
    skiplist_release_iterator(iter);

    mpd_del(q);
    mpd_del(r);
    mpd_del(price);
    mpd_del(amount);
    mpd_del(interval);

    json_t *result = json_object();
    json_object_set_new(result, "asks", asks);
    json_object_set_new(result, "bids", bids);

    return reply_result(ses, pkg, result);
}

static void svr_on_recv_pkg(nw_ses *ses, rpc_pkg *pkg)
{
    json_t *request = json_loadb(pkg->body, pkg->body_size, 0, NULL);
    if (request == NULL) {
        goto decode_error;
    }
    if (!json_is_array(request)) {
        goto decode_error;
    }

    int ret;
    switch (pkg->command) {
    case CMD_BALANCE_QUERY:
        log_trace("from: %s cmd balance query", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_balance_query(ses, pkg, request);
        if (ret < 0) {
            log_error("on_cmd_balance_query fail: %d", ret);
        }
        break;
    case CMD_BALANCE_UPDATE:
        log_trace("from: %s cmd balance update", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_balance_update(ses, pkg, request);
        if (ret < 0) {
            log_error("on_cmd_balance_update fail: %d", ret);
        }
        break;
    case CMD_ORDER_PUT_LIMIT:
        log_trace("from: %s cmd order put limit", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_order_put_limit(ses, pkg, request);
        if (ret < 0) {
            log_error("on_cmd_order_put_limit fail: %d", ret);
        }
        break;
    case CMD_ORDER_PUT_MARKET:
        log_trace("from: %s cmd order put market", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_order_put_market(ses, pkg, request);
        if (ret < 0) {
            log_error("on_cmd_order_put_market fail: %d", ret);
        }
        break;
    case CMD_ORDER_QUERY:
        log_trace("from: %s cmd order query", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_order_query(ses, pkg, request);
        if (ret < 0) {
            log_error("on_cmd_order_query fail: %d", ret);
        }
        break;
    case CMD_ORDER_CANCEL:
        log_trace("from: %s cmd order cancel", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_order_cancel(ses, pkg, request);
        if (ret < 0) {
            log_error("on_cmd_order_cancel fail: %d", ret);
        }
        break;
    case CMD_ORDER_BOOK:
        log_trace("from: %s cmd order book", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_order_book(ses, pkg, request);
        if (ret < 0) {
            log_error("on_cmd_order_book fail: %d", ret);
        }
        break;
    case CMD_ORDER_BOOK_DEPTH:
        log_trace("from: %s cmd order book depth", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_order_book_depth(ses, pkg, request);
        if (ret < 0) {
            log_error("on_cmd_order_book_depth fail: %d", ret);
        }
        break;
    case CMD_ORDER_BOOK_MERGE:
        log_trace("from: %s cmd order book merge", nw_sock_human_addr(&ses->peer_addr));
        ret = on_cmd_order_book_merge(ses, pkg, request);
        if (ret < 0) {
            log_error("on_cmd_order_book_merge fail: %d", ret);
        }
        break;
    default:
        log_error("from: %s unknown command: %u", nw_sock_human_addr(&ses->peer_addr), pkg->command);
        break;
    }

    json_decref(request);
    return;

decode_error:
    if (request) {
        json_decref(request);
    }
    sds hex = hexdump(pkg->body, pkg->body_size);
    log_error("connection: %s, cmd: %u decode request fail, request data: \n%s", \
            nw_sock_human_addr(&ses->peer_addr), pkg->command, hex);
    sdsfree(hex);
    rpc_svr_close_clt(svr, ses);

    return;
}

static void svr_on_new_connection(nw_ses *ses)
{
    log_info("new connection: %s", nw_sock_human_addr(&ses->peer_addr));
}

static void svr_on_connection_close(nw_ses *ses)
{
    log_info("connection: %s close", nw_sock_human_addr(&ses->peer_addr));
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

