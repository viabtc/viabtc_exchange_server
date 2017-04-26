/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/26, create
 */

# ifndef _UT_BASE64_H_
# define _UT_BASE64_H_

# include "ut_sds.h"
# include <stddef.h>

int base64_encode(const unsigned char *buffer, size_t length, sds *b64text);
int base64_decode(char *b64message, unsigned char **buffer, size_t *length);

# endif

