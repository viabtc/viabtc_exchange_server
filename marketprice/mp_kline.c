/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/18, create
 */

# include "mp_config.h"
# include "mp_kline.h"

struct kline_info *kline_info_new(mpd_t *open)
{
    struct kline_info *info = malloc(sizeof(struct kline_info));
    if (info == NULL)
        return NULL;

    info->open      = mpd_qncopy(open);
    info->close     = mpd_qncopy(open);
    info->high      = mpd_qncopy(open);
    info->low       = mpd_qncopy(open);
    info->volume    = mpd_qncopy(mpd_zero);
    info->deal      = mpd_qncopy(mpd_zero);

    return info;
}

void kline_info_update(struct kline_info *info, mpd_t *price, mpd_t *amount)
{
    mpd_t *deal = mpd_new(&mpd_ctx);
    mpd_mul(deal, price, amount, &mpd_ctx);
    mpd_copy(info->close, price, &mpd_ctx);
    mpd_add(info->volume, info->volume, amount, &mpd_ctx);
    mpd_add(info->deal, info->deal, deal, &mpd_ctx);
    if (mpd_cmp(price, info->high, &mpd_ctx) > 0)
        mpd_copy(info->high, price, &mpd_ctx);
    if (mpd_cmp(price, info->low, &mpd_ctx) < 0)
        mpd_copy(info->low, price, &mpd_ctx);
    mpd_del(deal);
}

void kline_info_merge(struct kline_info *info, struct kline_info *update)
{
    mpd_copy(info->close, update->close, &mpd_ctx);
    mpd_add(info->volume, info->volume, update->volume, &mpd_ctx);
    mpd_add(info->deal, info->deal, update->deal, &mpd_ctx);
    if (mpd_cmp(update->high, info->high, &mpd_ctx) > 0)
        mpd_copy(info->high, update->high, &mpd_ctx);
    if (mpd_cmp(update->low, info->low, &mpd_ctx) < 0)
        mpd_copy(info->low, update->low, &mpd_ctx);
}

void kline_info_free(struct kline_info *info)
{
    if (info->open)
        mpd_del(info->open);
    if (info->close)
        mpd_del(info->close);
    if (info->high)
        mpd_del(info->high);
    if (info->low)
        mpd_del(info->low);
    if (info->volume)
        mpd_del(info->volume);
    if (info->deal)
        mpd_del(info->deal);
    free(info);
}

struct kline_info *kline_from_str(char *str)
{
    json_t *obj = json_loadb(str, strlen(str), 0, NULL);
    if (obj == NULL) {
        return NULL;
    }
    if (!json_is_array(obj) || json_array_size(obj) < 5) {
        json_decref(obj);
        return NULL;
    }
    struct kline_info *info = malloc(sizeof(struct kline_info));
    if (info == NULL) {
        json_decref(obj);
        return NULL;
    }
    memset(info, 0, sizeof(struct kline_info));

    const char *open = json_string_value(json_array_get(obj, 0));
    if (!open || (info->open = decimal(open, 0)) == NULL) {
        goto cleanup;
    }
    const char *close = json_string_value(json_array_get(obj, 1));
    if (!close || (info->close = decimal(close, 0)) == NULL) {
        goto cleanup;
    }
    const char *high = json_string_value(json_array_get(obj, 2));
    if (!high || (info->high = decimal(high, 0)) == NULL) {
        goto cleanup;
    }
    const char *low = json_string_value(json_array_get(obj, 3));
    if (!low || (info->low = decimal(low, 0)) == NULL) {
        goto cleanup;
    }
    const char *volume = json_string_value(json_array_get(obj, 4));
    if (!volume || (info->volume = decimal(volume, 0)) == NULL) {
        goto cleanup;
    }

    if (json_array_size(obj) >= 6) {
        const char *deal = json_string_value(json_array_get(obj, 5));
        if (!deal || (info->deal = decimal(deal, 0)) == NULL) {
            goto cleanup;
        }
    } else {
        info->deal = mpd_qncopy(mpd_zero);
    }

    json_decref(obj);
    return info;

cleanup:
    json_decref(obj);
    kline_info_free(info);
    return NULL;
}

char *kline_to_str(struct kline_info *info)
{
    json_t *obj = json_array();
    json_array_append_new_mpd(obj, info->open);
    json_array_append_new_mpd(obj, info->close);
    json_array_append_new_mpd(obj, info->high);
    json_array_append_new_mpd(obj, info->low);
    json_array_append_new_mpd(obj, info->volume);
    json_array_append_new_mpd(obj, info->deal);
    char *str = json_dumps(obj, 0);
    json_decref(obj);
    return str;
}

