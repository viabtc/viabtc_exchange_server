/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/18, create
 */

# ifndef _MP_KLINE_H_
# define _MP_KLINE_H_

# include <stdint.h>
# include "ut_decimal.h"

struct kline_info {
    mpd_t *open;
    mpd_t *close;
    mpd_t *high;
    mpd_t *low;
    mpd_t *volume;
    mpd_t *deal;
};

struct kline_info *kline_info_new(mpd_t *open);
struct kline_info *kline_from_str(char *str);
void kline_info_update(struct kline_info *info, mpd_t *price, mpd_t *amount);
void kline_info_merge(struct kline_info *info, struct kline_info *update);
void kline_info_free(struct kline_info *info);
char *kline_to_str(struct kline_info *info);

# endif

