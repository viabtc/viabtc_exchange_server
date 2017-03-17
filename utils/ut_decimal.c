/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/17, create
 */

# include "ut_decimal.h"

mpd_context_t mpd_ctx;
mpd_t *mpd_zero;

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

