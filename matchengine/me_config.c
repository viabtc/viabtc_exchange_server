/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/16, create
 */

# include "me_config.h"

struct settings settings;
mpd_context_t mpd_ctx;
mpd_t *mpd_zero;

static int load_assets(json_t *root, const char *key)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_array(node)) {
        return -__LINE__;
    }

    settings.asset_num = json_array_size(node);
    settings.assets = malloc(sizeof(struct asset) * settings.asset_num);
    for (size_t i = 0; i < settings.asset_num; ++i) {
        json_t *row = json_array_get(node, i);
        if (!json_is_object(row))
            return -__LINE__;
        ERR_RET_LN(read_cfg_str(row, "name", &settings.assets[i].name, NULL));
        ERR_RET_LN(read_cfg_uint32(row, "prec", &settings.assets[i].prec, true, 0));
        if (strlen(settings.assets[i].name) > ASSET_NAME_MAX_LEN)
            return -__LINE__;
    }

    return 0;
}

static int load_markets(json_t *root, const char *key)
{
    json_t *node = json_object_get(root, key);
    if (!node || !json_is_array(node)) {
        return -__LINE__;
    }

    settings.market_num = json_array_size(node);
    settings.markets = malloc(sizeof(struct market) * settings.market_num);
    for (size_t i = 0; i < settings.market_num; ++i) {
        json_t *row = json_array_get(node, i);
        if (!json_is_object(row))
            return -__LINE__;
        ERR_RET_LN(read_cfg_str(row, "name", &settings.markets[i].name, NULL));
        ERR_RET_LN(read_cfg_str(row, "stock", &settings.markets[i].stock, NULL));
        ERR_RET_LN(read_cfg_str(row, "money", &settings.markets[i].money, NULL));
    }

    return 0;
}

static int read_config_from_json(json_t *root)
{
    int ret;
    ret = load_cfg_process(root, "process", &settings.process);
    if (ret < 0) {
        printf("load process config fail: %d\n", ret);
        return -__LINE__;
    }
    ret = load_cfg_log(root, "log", &settings.log);
    if (ret < 0) {
        printf("load log config fail: %d\n", ret);
        return -__LINE__;
    }
    ret = load_cfg_svr(root, "svr", &settings.svr);
    if (ret < 0) {
        printf("load svr config fail: %d\n", ret);
        return -__LINE__;
    }
    ret = load_assets(root, "assets");
    if (ret < 0) {
        printf("load assets config fail: %d\n", ret);
        return -__LINE__;
    }
    ret = load_markets(root, "markets");
    if (ret < 0) {
        printf("load markets config fail: %d\n", ret);
        return -__LINE__;
    }

    return 0;
}

int init_config(const char *path)
{
    json_error_t error;
    json_t *root = json_load_file(path, 0, &error);
    if (root == NULL) {
        printf("json_load_file from: %s fail: %s in line: %d\n", path, error.text, error.line);
        return -__LINE__;
    }
    if (!json_is_object(root)) {
        json_decref(root);
        return -__LINE__;
    }

    int ret = read_config_from_json(root);
    if (ret < 0) {
        json_decref(root);
        return ret;
    }
    json_decref(root);

    return 0;
}

int init_mpd(void)
{
    mpd_ieee_context(&mpd_ctx, MPD_DECIMAL128);
    mpd_zero = mpd_new(&mpd_ctx);
    mpd_set_string(mpd_zero, "0", &mpd_ctx);

    return 0;
}

mpd_t *quantize(mpd_t *val, int num)
{
    mpd_t *tmp = mpd_new(&mpd_ctx);
    mpd_copy(tmp, val, &mpd_ctx);
    mpd_rescale(val, tmp, -num, &mpd_ctx);
    mpd_del(tmp);
    return val;
}

