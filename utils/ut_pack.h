/*
 * Description: 
 *     History: yang@haipo.me, 2016/04/09, create
 */

# ifndef _UT_PACK_
# define _UT_PACK_

# include <stdint.h>
# include <stddef.h>

# include "ut_sds.h"

int pack_varint_le(void **dest, size_t *left, uint64_t num);
int unpack_varint_le(void **src, size_t *left, uint64_t *num);

int pack_uint16_le(void **dest, size_t *left, uint16_t num);
int unpack_uint16_le(void **src, size_t *left, uint16_t *num);

int pack_uint32_le(void **dest, size_t *left, uint32_t num);
int unpack_uint32_le(void **src, size_t *left, uint32_t *num);

int pack_uint64_le(void **dest, size_t *left, uint64_t num);
int unpack_uint64_le(void **src, size_t *left, uint64_t *num);

int pack_varstr(void **dest, size_t *left, const char *str, size_t len);
int unpack_varstr(void **src, size_t *left, sds *str);

int pack_buf(void **dest, size_t *left, const void *data, size_t len);
int unpack_buf(void **src, size_t *left, void *dest, size_t len);

int pack_char(void **dest, size_t *left, uint8_t c);
int unpack_char(void **src, size_t *left, uint8_t *c);

int pack_oppush(void **dest, size_t *left, const char *data, size_t len);
int unpack_oppush(void **src, size_t *left, char *dest, size_t *size);

int pack_oppushint_le(void **dest, size_t *left, int64_t num);
int unpack_oppushint_le(void **src, size_t *left, int64_t *num);

# endif

