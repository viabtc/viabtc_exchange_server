/*
 * Description: misc functions
 *     History: yang@haipo.me, 2016/03/15, create
 */

# ifndef _GNU_SOURCE
# define _GNU_SOURCE
# endif

# include <stdio.h>
# include <stdint.h>
# include <string.h>
# include <stdlib.h>
# include <stdarg.h>
# include <stdbool.h>
# include <limits.h>
# include <ctype.h>
# include <math.h>
# include <errno.h>
# include <unistd.h>
# include <signal.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/time.h>
# include <sys/file.h>
# include <sys/resource.h>

# include "ut_misc.h"
# include "ut_signal.h"

int process_exist(const char *fmt, ...)
{
    char name[PATH_MAX];
	va_list ap;
    va_start(ap, fmt);
    vsnprintf(name, sizeof(name), fmt, ap);
    va_end(ap);

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "/tmp/%s.lock", name);

    int fd = open(path, O_CREAT, 400);
    if (fd < 0)
        return -1;
    if (flock(fd, LOCK_EX | LOCK_NB) < 0)
        return 1;

    return 0;
}

int process_keepalive(void)
{
    while (true) {
        int pid = fork();
        if (pid < 0) {
            log_error("fork error: %d: %s", pid, strerror(errno));
            return -1;
        } else if (pid == 0 ) {
            init_signal();
            return 0;
        } else {
            init_signal();
            signal(SIGCHLD, SIG_DFL);
            signal(SIGUSR1, SIG_IGN);
            signal(SIGTTIN, SIG_IGN);
            signal(SIGTTOU, SIG_IGN);
            int status = 0;
            int ret = waitpid(pid, &status, 0);
            if (ret < 0) {
                if (signal_exit) {
                    exit(EXIT_SUCCESS);
                } else {
                    log_error("waitpid: %d error: %d: %s", pid, ret, strerror(errno));
                    exit(EXIT_FAILURE);
                }
            }
            if (WIFEXITED(status)) {
                exit(EXIT_SUCCESS);
            } else if (WIFSIGNALED(status)) {
                log_fatal("process: %d, name: %s terminated by signal: '%s'", \
                        pid, program_invocation_short_name, strsignal(WTERMSIG(status)));
                usleep(1000 * 1000);
                continue;
            } else {
                log_error("process: %d terminated, waitpid status: %d\n", pid, status);
                exit(EXIT_FAILURE);
            }
        }
    }

    return -1;
}

int set_core_limit(size_t limit)
{
    struct rlimit rlim;
    memset(&rlim, 0, sizeof(rlim));
    if (getrlimit(RLIMIT_CORE, &rlim) < 0) {
        return -1;
    }
    if (rlim.rlim_cur >= limit)
        return 0;
    rlim.rlim_cur = limit;
    rlim.rlim_max = limit;
    if (setrlimit(RLIMIT_CORE, &rlim) < 0) {
        return -1;
    }

    return 0;
}

int set_file_limit(size_t limit)
{
    struct rlimit rlim;
    memset(&rlim, 0, sizeof(rlim));
    if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
        return -1;
    }
    if (rlim.rlim_cur >= limit)
        return 0;
    rlim.rlim_cur = limit;
    rlim.rlim_max = limit;
    if (setrlimit(RLIMIT_NOFILE, &rlim) < 0) {
        return -1;
    }

    return 0;
}

sds hexdump(const void *mem, size_t len)
{
    uint8_t *data = (uint8_t *)mem;
    size_t pos = 0;
    sds result = sdsempty();

    while (pos < len) {
        char line[100];
        size_t ret = 0;
        size_t left   = len - pos;
        size_t num    = left >= 16 ? 16 : left;
        size_t group  = num / 2;
        size_t single = num % 2;
        const char *start = (char *)(data + pos);

        ret += snprintf(line + ret, sizeof(line) - ret, "0x%04zx:  ", pos);
        for (size_t i = 0; i < group; ++i) {
            uint8_t const *curr = data + pos;
            ret += snprintf(line + ret, sizeof(line) - ret,  \
                    "%02x%02x ", curr[0], curr[1]);
            pos += 2;
        }
        if (group != 8) {
            if (single) {
                ret += snprintf(line + ret, sizeof(line) - ret, \
                        "%02x   ", data[pos++]);
            }
            size_t blank = 8 - group - single;
            for (size_t i = 0; i < blank; ++i) {
                ret += snprintf(line + ret, sizeof(line) - ret, "     ");
            }
        }
        ret += snprintf(line + ret, sizeof(line) - ret, " ");
        for (size_t i = 0; i < num; ++i) {
            char c = start[i];
            ret += snprintf(line + ret, sizeof(line) - ret, "%c", \
                    (c > (char)0x20 && c < (char)0x7f) ? c : '.');
        }
        if (pos < len) {
            snprintf(line + ret, sizeof(line) - ret, "\n");
        }

        result = sdscat(result, line);
    }

    return result;
}

sds bin2hex(const void *mem, size_t len)
{
    static char hex[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
    };
    sds result = sdsempty();
    for (size_t i = 0; i < len; ++i) {
        char s[2];
        uint8_t c = *(uint8_t *)(mem + i);
        s[0] = hex[(c & 0xf0) >> 4];
        s[1] = hex[(c & 0x0f)];
        result = sdscatlen(result, s, 2);
    }
    return result;
}

static int hex2int(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

sds hex2bin(const char *hex)
{
    size_t len = strlen(hex);
    if (len % 2 != 0)
        return NULL;

    sds result = sdsempty();
    size_t step = len / 2;
    for (size_t i = 0; i < step; ++i) {
        int part1 = hex2int(hex[i * 2]);
        if (part1 < 0) {
            sdsfree(result);
            return NULL;
        }
        int part2 = hex2int(hex[i * 2 + 1]);
        if (part2 < 0) {
            sdsfree(result);
            return NULL;
        }
        uint8_t c = ((uint8_t)part1) << 4 | (uint8_t)part2;
        result = sdscatlen(result, &c, 1);
    }

    return result;
}

double current_timestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
}

char *strftimestamp(time_t t)
{
    static char str[64];
    struct tm *tm = localtime(&t);
    snprintf(str, sizeof(str), "%04d-%02d-%02d %02d:%02d:%02d",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    return str;
}

char *human_number(double num)
{
    static char str[20];
    const char *units[] = {"", "K", "M", "G", "T"};
    for (int i = 0; i < 5; ++i) {
        if (num < 1000) {
            snprintf(str, sizeof(str), "%.3f%s", num, units[i]);
            return str;
        }
        num /= 1000;
    }
    snprintf(str, sizeof(str), "%.3fP", num);
    return str;
}

double to_fixed(double val, int num)
{
    double multiplier = 1.0;
    for (int i = 0; i < num; ++i) {
        multiplier *= 10;
    }
    return round(val * multiplier) / multiplier;
}

void reverse_mem(char *mem, size_t len)
{
    size_t step = len / 2;
    for (size_t i = 0; i < step; ++i) {
        char tmp = mem[i];
        mem[i] = mem[len - 1 - i];
        mem[len - 1 - i] = tmp;
    }
}

void strtolower(char *str)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; ++i) {
        str[i] = tolower(str[i]);
    }
}

void strtoupper(char *str)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; ++i) {
        str[i] = toupper(str[i]);
    }
}

void strclearblank(char *str)
{
    size_t len = strlen(str);
    size_t pos = 0;
    for (size_t i = 0; i < len; ++i) {
        if (!isblank(str[i])) {
            str[pos++] = str[i];
        }
    }
    str[pos] = '\0';
}

int urandom(void *buf, size_t size)
{
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
        return -1;
    int n = read(fd, buf, size);
    if (n < 0)
        return -1;
    close(fd);
    return n;
}

char *sstrncpy(char *dest, const char *src, size_t n)
{
    if (n == 0)
        return dest;
    dest[0] = 0;
    return strncat(dest, src, n - 1);
}

time_t get_timezone_offset(void)
{
    time_t timestamp = 0;
    struct tm *timeinfo = localtime(&timestamp);
    if (timeinfo->tm_mday == 1) {
        return -timeinfo->tm_hour * 3600;
    } else {
        return (24 - timeinfo->tm_hour) * 3600;
    }
}

