/*
 * Description: 
 *     History: yang@haipo.me, 2016/04/09, create
 */

# include <string.h>
# include <stdlib.h>
# include <stdbool.h>

# include "ut_misc.h"
# include "ut_pack.h"

int pack_varint_le(void **dest, size_t *left, uint64_t num)
{
    int space = 0;
    if (num < 0xfd) {
        space = 1;
        if (*left < space)
            return -1;
        *(uint8_t *)(*dest) = num;
    } else if (num < 0xffff) {
        space = 3;
        if (*left < space)
            return -1;
        *(uint8_t *)(*dest) = 0xfd;
        uint16_t tmp = htole64((uint16_t)num);
        memcpy(*dest + 1, &tmp, sizeof(tmp));
    } else if (num < 0xffffffffu) {
        space = 5;
        if (*left < space)
            return -1;
        *(uint8_t *)(*dest) = 0xfe;
        uint32_t tmp = htole32((uint32_t)num);
        memcpy(*dest + 1, &tmp, sizeof(tmp));
    } else {
        space = 9;
        if (*left < space)
            return -1;
        *(uint8_t *)(*dest) = 0xff;
        uint64_t tmp = htole64(num);
        memcpy(*dest + 1, &tmp, sizeof(tmp));
    }

    *dest += space;
    *left -= space;
    return space;
}

int unpack_varint_le(void **src, size_t *left, uint64_t *num)
{
    if (*left < 1)
        return -1;
    int space = 0;
    uint8_t mark = *(uint8_t *)(*src);
    if (mark < 0xfd) {
        space = 1;
        *num = mark;
    } else if (mark == 0xfd) {
        space = 3;
        if (*left < space)
            return -1;
        *num = le16toh(*(uint16_t *)(*src + 1));
    } else if (mark == 0xfe) {
        space = 5;
        if (*left < space)
            return -1;
        *num = le32toh(*(uint32_t *)(*src + 1));
    } else {
        space = 9;
        if (*left < space)
            return -1;
        *num = le64toh(*(uint64_t *)(*src + 1));
    }

    *src  += space;
    *left -= space;
    return space;
}

int pack_uint16_le(void **dest, size_t *left, uint16_t num)
{
    if (*left < 4)
        return -1;
    *(uint16_t *)(*dest) = htole16(num);
    *dest += 2;
    *left -= 2;
    return 2;
}

int unpack_uint16_le(void **src, size_t *left, uint16_t *num)
{
    if (*left < 4)
        return -1;
    *num = le16toh(*(uint16_t *)(*src));
    *src  += 2;
    *left -= 2;
    return 2;
}

int pack_uint32_le(void **dest, size_t *left, uint32_t num)
{
    if (*left < 4)
        return -1;
    *(uint32_t *)(*dest) = htole32(num);
    *dest += 4;
    *left -= 4;
    return 4;
}

int unpack_uint32_le(void **src, size_t *left, uint32_t *num)
{
    if (*left < 4)
        return -1;
    *num = le32toh(*(uint32_t *)(*src));
    *src  += 4;
    *left -= 4;
    return 4;
}

int pack_uint64_le(void **dest, size_t *left, uint64_t num)
{
    if (*left < 8)
        return -1;
    *(uint64_t *)(*dest) = htole64(num);
    *dest += 8;
    *left -= 8;
    return 8;
}

int unpack_uint64_le(void **src, size_t *left, uint64_t *num)
{
    if (*left < 8)
        return -1;
    *num = le64toh(*(uint64_t *)(*src));
    *src  += 8;
    *left -= 8;
    return 8;
}

int pack_varstr(void **dest, size_t *left, const char *str, size_t len)
{
    int ret = pack_varint_le(dest, left, len);
    if (ret < 0)
        return -1;
    if (*left < len)
        return -1;
    memcpy(*dest, str, len);
    *dest += len;
    *left -= len;
    return ret + len;
}

int unpack_varstr(void **src, size_t *left, sds *str)
{
    uint64_t len;
    int ret = unpack_varint_le(src, left, &len);
    if (ret < 0)
        return -1;
    if (*left < len)
        return -1;
    *str = sdsempty();
    *str = sdscatlen(*str, *src, len);
    *src  += len;
    *left -= len;
    return ret + len;
}

int pack_buf(void **dest, size_t *left, const void *data, size_t len)
{
    if (*left < len)
        return -1;
    memcpy(*dest, data, len);
    *dest += len;
    *left -= len;
    return len;
}

int unpack_buf(void **src, size_t *left, void *dest, size_t len)
{
    if (*left < len)
        return -1;
    memcpy(dest, *src, len);
    *src  += len;
    *left -= len;
    return len;
}

int pack_char(void **dest, size_t *left, uint8_t c)
{
    if (*left < 1)
        return -1;
    *(uint8_t *)(*dest) = c;
    *dest += 1;
    *left -= 1;
    return 1;
}

int unpack_char(void **src, size_t *left, uint8_t *c)
{
    if (*left < 1)
        return -1;
    *c = *(uint8_t *)(*src);
    *src  += 1;
    *left -= 1;
    return 1;
}

int pack_oppush(void **dest, size_t *left, const char *data, size_t len)
{
    size_t pos = *left;
    if (len <= 0x4b) {
        if (*left < (1 + len))
            return -1;
        pack_char(dest, left, len);
    } else if (len <= 0xff) {
        if (*left < (2 + len))
            return -1;
        pack_char(dest, left, 0x4c);
        pack_char(dest, left, len);
    } else if (len <= 0xffff) {
        if (*left < (3 + len))
            return -1;
        pack_char(dest, left, 0x4d);
        pack_uint16_le(dest, left, len);
    } else if (len <= 0xffffffff) {
        if (*left < (5 + len))
            return -1;
        pack_char(dest, left, 0x4e);
        pack_uint32_le(dest, left, len);
    } else {
        return -1;
    }

    pack_buf(dest, left, data, len);
    return pos - *left;
}

int unpack_oppush(void **src, size_t *left, char *dest, size_t *size)
{
    size_t pos = *left;
    size_t len = 0;
    uint8_t op;
    if (unpack_char(src, left, &op) < 0)
        return -1;
    if (op <= 0x4b) {
        len = op;
    } else if (op == 0x4c) {
        uint8_t data_len;
        if (unpack_char(src, left, &data_len) < 0)
            return -1;
        len = data_len;
    } else if (op == 0x4d) {
        uint16_t data_len;
        if (unpack_uint16_le(src, left, &data_len) < 0)
            return -1;
        len = data_len;
    } else if (op == 0x4e) {
        uint32_t data_len;
        if (unpack_uint32_le(src, left, &data_len) < 0)
            return -1;
        len = data_len;
    } else {
        return -1;
    }
    if (*left < len || *size < len)
        return -1;
    *size = len;

    unpack_buf(src, left, dest, len);
    return pos - *left;
}

int pack_oppushint_le(void **dest, size_t *left, int64_t num)
{
    if (num == INT64_MIN) {
        return -1;
    } else if (num == -1) {
        return pack_char(dest, left, 0x4f);
    } else if (num == 0) {
        return pack_char(dest, left, 0x00);
    } else if (num >= 1 && num <= 16) {
        return pack_char(dest, left, 0x50 + num);
    }

    bool neg = num < 0;
    uint64_t absvalue = neg ? -num : num;
    uint8_t buf[8];
    size_t len = 0;

    while (absvalue) {
        buf[len++] = absvalue & 0xff;
        absvalue >>= 8;
    }

    if (buf[len - 1] & 0x80) {
        buf[len++] = neg ? 0x80 : 0x00;
    } else if (neg) {
        buf[len - 1] |= 0x80;
    }

    return pack_oppush(dest, left, (char *)buf, len);
}

int unpack_oppushint_le(void **src, size_t *left, int64_t *num)
{
    if (*left < 1)
        return -1;
    uint8_t op = *(uint8_t *)(*src);
    if (op == 0x00) {
        *num = 0;
        return unpack_char(src, left, &op);
    } else if (op == 0x4f) {
        *num = -1;
        return unpack_char(src, left, &op);
    } else if (op > 0x50 && op <= 0x60) {
        *num = op - 0x50;
        return unpack_char(src, left, &op);
    } else if (op > sizeof(int64_t)) {
        return -1;
    }

    uint8_t buf[8] = { 0 };
    size_t len = sizeof(buf);
    int space = unpack_oppush(src, left, (char *)buf, &len);
    if (space < 0)
        return space;

    bool neg = buf[len - 1] & 0x80;
    buf[len - 1] &= ~0x80;
    int64_t absvalue = 0;
    for (int i = len - 1; i >= 0; --i) {
        absvalue <<= 8;
        absvalue += buf[i];
    }
    *num = neg ? -absvalue : absvalue;

    return space;
}

