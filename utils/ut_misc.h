/*
 * Description: misc functions
 *     History: yang@haipo.me, 2016/03/15, create
 */

# ifndef _MISC_H_
# define _MISC_H_

# include <endian.h>
# include <byteswap.h>

# include "ut_sds.h"
# include "ut_log.h"

int process_exist(const char *fmt, ...);
int process_keepalive(void);

int set_core_limit(size_t limit);
int set_file_limit(size_t limit);

sds hexdump(const void *mem, size_t len);
sds bin2hex(const void *mem, size_t len);
sds hex2bin(const char *hex);

double current_timestamp(void);
char *strftimestamp(time_t t);
char *human_number(double num);

double to_fixed(double val, int num);
void reverse_mem(char *mem, size_t len);

void strtolower(char *str);
void strtoupper(char *str);
void strclearblank(char *str);

int urandom(void *buf, size_t size);

char *sstrncpy(char *dest, const char *src, size_t n);
time_t get_timezone_offset(void);

# undef ERR_RET
# define ERR_RET(x) do { \
    int __ret = (x); \
    if (__ret < 0) { \
        return __ret; \
    } \
} while (0)

# undef ERR_RET_LN
# define ERR_RET_LN(x) do { \
    if ((x) < 0) { \
        return -__LINE__; \
    } \
} while (0)

# undef htobe16
# undef htobe32
# undef htobe64
# undef be16toh
# undef be32toh
# undef be64toh
# undef htole16
# undef htole32
# undef htole64
# undef le16toh
# undef le32toh
# undef le64toh

# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define htole16(x) (x)
#  define htole32(x) (x)
#  define htole64(x) (x)
#  define le16toh(x) (x)
#  define le32toh(x) (x)
#  define le64toh(x) (x)
#  define htobe16(x) bswap_16(x)
#  define htobe32(x) bswap_32(x)
#  define htobe64(x) bswap_64(x)
#  define be16toh(x) bswap_16(x)
#  define be32toh(x) bswap_32(x)
#  define be64toh(x) bswap_64(x)
# else
#  define htole16(x) bswap_16(x)
#  define htole32(x) bswap_32(x)
#  define htole64(x) bswap_64(x)
#  define le16toh(x) bswap_16(x)
#  define le32toh(x) bswap_32(x)
#  define le64toh(x) bswap_64(x)
#  define htobe16(x) (x)
#  define htobe32(x) (x)
#  define htobe64(x) (x)
#  define be16toh(x) (x)
#  define be32toh(x) (x)
#  define be64toh(x) (x)
# endif

# endif

