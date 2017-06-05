/*
 * Description: 
 *     History: yang@haipo.me, 2017/06/05, create
 */

# include <stdio.h>
# include "ut_decimal.h"
# include "ut_misc.h"

int main(int argc, char *argv[])
{
    init_mpd();
    mpd_t *a = decimal("100.12345678", 0);
    mpd_t *b = decimal("200.12345678", 0);
    mpd_t *c = mpd_new(&mpd_ctx);

    double start = current_timestamp();
    for (int i = 0; i < 1000000; ++i) {
        mpd_add(c, a, b, &mpd_ctx);
    }
    double end = current_timestamp();
    printf("%f\n", end - start);

    return 0;
}

