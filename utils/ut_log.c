/*
 * Description: a log lib with cache, writen by damon
 *     History: yang@haipo.me, 2013/05/20, created
 */

# undef  _FILE_OFFSET_BITS
# define _FILE_OFFSET_BITS 64

# include <stdio.h>
# include <stdint.h>
# include <string.h>
# include <stdlib.h>
# include <stdbool.h>
# include <stdarg.h>
# include <fcntl.h>
# include <time.h>
# include <errno.h>
# include <limits.h>
# include <unistd.h>
# include <inttypes.h>
# include <libgen.h>
# include <syslog.h>
# include <execinfo.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# include "ut_log.h"

dlog_t   *default_dlog;
int       default_dlog_flag = 0xff;

dlog_on_fatal_cb dlog_on_fatal;

# define WRITE_INTERVAL_IN_USEC (10 * 1000)     /* 100 ms */
# define WRITE_BUFFER_CHECK_LEN (32 * 1024)     /* 32 KB */
# define WRITE_BUFFER_LEN       (64 * 1024)     /* 64 KB */

/* all opened log is in a list, vist by log_list_head */
static dlog_t *log_list_head = NULL;

/* use to make sure dlog_atexit only call once */
static int dlog_init_flag = 0;

static char *log_suffix(int type, time_t time, int index)
{
    static char str[30];
    if (type == DLOG_SHIFT_BY_SIZE) {
        if (index) {
            snprintf(str, sizeof(str), ".log.%d", index);
        } else {
            snprintf(str, sizeof(str), ".log");
        }
        return str;
    }

    struct tm *t = localtime(&time);
    ssize_t n = 0;
    switch (type)
    {
    case DLOG_SHIFT_BY_MIN:
        n = snprintf(str, sizeof(str), ".%04d%02d%02d%02d%02d",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
        break;
    case DLOG_SHIFT_BY_HOUR:
        n = snprintf(str, sizeof(str), ".%04d%02d%02d%02d",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour);
        break;
    case DLOG_SHIFT_BY_DAY:
        n = snprintf(str, sizeof(str), ".%04d%02d%02d",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
        break;
    default:
        str[0] = 0;
        break;
    }

    if (index) {
        snprintf(str + n, sizeof(str) - n, ".log.%d", index);
    } else {
        snprintf(str + n, sizeof(str) - n, ".log");
    }

    return str;
}

static inline uint64_t timeval_diff(struct timeval *old, struct timeval *new)
{
    return (new->tv_sec - old->tv_sec) * (1000 * 1000) - (old->tv_usec - new->tv_usec);
}

static int inner_unlink_expire(dlog_t *log, time_t expire_time)
{
    char path[PATH_MAX];
    int  num = 0;
    int  i;

    if (log->log_num == 0) {
        for (i = 0; ; ++i) {
            snprintf(path, PATH_MAX, "%s%s", log->base_name, log_suffix(log->shift_type, expire_time, i));
            if (access(path, F_OK) == 0) {
                ++num;
            } else {
                break;
            }
        }
    } else {
        num = log->log_num;
    }

    for (i = 0; i < num; ++i) {
        snprintf(path, PATH_MAX, "%s%s", log->base_name, log_suffix(log->shift_type, expire_time, i));
        unlink(path);
    }

    return 0;
}

static int unlink_expire(dlog_t *log, struct timeval *now)
{
    if (log->shift_type == DLOG_SHIFT_BY_SIZE || log->keep_time == 0)
        return 0;

    int expire = 0;
    time_t expire_time;

    switch (log->shift_type) {
    case DLOG_SHIFT_BY_MIN:
        if (now->tv_sec - log->last_unlink > 60) {
            expire = 1;
            expire_time = now->tv_sec - 60 * log->keep_time;
        }
        break;
    case DLOG_SHIFT_BY_HOUR:
        if (now->tv_sec - log->last_unlink > 3600) {
            expire = 1;
            expire_time = now->tv_sec - 3600 * log->keep_time;
        }
        break;
    case DLOG_SHIFT_BY_DAY:
        if (now->tv_sec - log->last_unlink > 86400) {
            expire = 1;
            expire_time = now->tv_sec - 86400 * log->keep_time;
        }
        break;
    }

    if (expire) {
        log->last_unlink = now->tv_sec;
        if (log->use_fork) {
            if (fork() == 0) {
                inner_unlink_expire(log, expire_time);
                _exit(0);
            }
        } else {
            inner_unlink_expire(log, expire_time);
        }
    }

    return 0;
}

static char *get_log_name(dlog_t *log, struct timeval *now)
{
    sprintf(log->name, "%s%s", log->base_name, log_suffix(log->shift_type, now->tv_sec, 0));
    return log->name;
}

static int inner_shift_log(dlog_t *log, struct timeval *now)
{
    if (log->log_num == 1) {
        unlink(log->name);
        return 0;
    }

    char path[PATH_MAX];
    char new_path[PATH_MAX];
    int  num = 0;
    int  i;

    if (log->log_num == 0) {
        for (i = 0; ; ++i) {
            snprintf(path, PATH_MAX, "%s%s", log->base_name, log_suffix(log->shift_type, now->tv_sec, i));
            if (access(path, F_OK) == 0) {
                ++num;
            } else {
                break;
            }
        }
    } else {
        num = log->log_num - 1;
    }

    for (i = num - 1; i >= 0; --i) {
        snprintf(path, PATH_MAX, "%s%s", log->base_name, log_suffix(log->shift_type, now->tv_sec, i));
        if (access(path, F_OK) == 0) {
            snprintf(new_path, PATH_MAX, "%s%s", log->base_name, log_suffix(log->shift_type, now->tv_sec, i + 1));
            rename(path, new_path);
        }
    }

    return 0;
}

static int shift_log(dlog_t *log, struct timeval *now)
{
    if (log->max_size == 0)
        return 0;
    if (log->use_fork && ((now->tv_sec - log->last_shift) <= 3))
        return 0;

    struct stat fs;
    if (stat(log->name, &fs) < 0) {
        if (errno == ENOENT) {
            return 0;
        } else {
            return -1;
        }
    }

    int ret = 0;
    if (fs.st_size >= log->max_size) {
        log->last_shift = now->tv_sec;
        if (log->use_fork) {
            if (fork() == 0) {
                inner_shift_log(log, now);
                _exit(0);
            }
        } else {
            inner_shift_log(log, now);
        }
    }

    return ret;
}

static ssize_t xwrite(int fd, const void *buf, size_t len)
{
    ssize_t nr;
    while (1) {
        nr = write(fd, buf, len);
        if ((nr < 0) && (errno == EAGAIN || errno == EINTR)) {
            continue;
        }
        return nr;
    }
}

static ssize_t write_in_full(int fd, const void *buf, size_t size)
{
    const char *p = buf;
    ssize_t total = 0;
    while (size > 0) {
        ssize_t written = xwrite(fd, p, size);
        if (written < 0)
            return -1;
        if (!written) {
            errno = ENOSPC;
            return -1;
        }
        size -= written;
        p += written;
        total += written;
    }

    return total;
}

static int flush_log(dlog_t *log, struct timeval *now)
{
    if (log->write_len == 0)
        return 0;

    ssize_t n = 0;
    int ret_val = 0;
    if (log->remote_log) {
        if (log->sockfd < 0) {
            int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd < 0) {
                ret_val = -1;
                goto end_lable;
            }
            log->sockfd = sockfd;
        }
        n = sendto(log->sockfd, log->buf, log->write_len, 0, (struct sockaddr *)&log->addr, sizeof(log->addr));
        if (n < 0) {
            if (errno == EBADF) {
                log->sockfd = -1;
                log->external_sockfd = 0;
            }
            ret_val = -1;
            goto end_lable;
        }
    } else {
        get_log_name(log, now);
        if (!log->no_shift) {
            shift_log(log, now);
            unlink_expire(log, now);
        }
        int fd = open(log->name, O_WRONLY | O_APPEND | O_CREAT, 0664);
        if (fd < 0) {
            ret_val = -1;
            goto end_lable;
        }
        n = write_in_full(fd, log->buf, log->write_len);
        close(fd);
        if (n < 0) {
            ret_val = -1;
            goto end_lable;
        }
    }

end_lable:
    log->write_len = 0;
    log->last_write = *now;

    return ret_val;
}

static void dlog_atexit(void)
{
    dlog_t *log = log_list_head;
    while (log) {
        dlog_t *tmp_log = log;
        log = (dlog_t *)log->next;
        dlog_fini(tmp_log);
    }
}

static void *dlog_free(dlog_t *log)
{
    if (log->remote_log && log->external_sockfd == 0 && log->sockfd >= 0) {
        close(log->sockfd);
    }
    free(log->base_name);
    free(log->name);
    free(log->buf);
    free(log);

    return NULL;
}

static int is_remote_log(const char *base_name, struct sockaddr_in *addr)
{
    if (strchr(base_name, ':')) {
        char name[PATH_MAX] = { 0 };
        strncpy(name, base_name, sizeof(name) - 1);
        char *ip = strtok(name, ": \t\n\r");
        char *port = strtok(NULL, ": \t\n\r");
        if (ip == NULL || port == NULL)
            return -1;
        memset(addr, 0, sizeof(struct sockaddr_in));
        addr->sin_family = AF_INET;
        if (inet_aton(ip, &addr->sin_addr) == 0)
            return -1;
        addr->sin_port = htons((unsigned short)atoi(port));
        if (addr->sin_port == 0)
            return -1;
        return 1;
    }

    return 0;
}

dlog_t *dlog_init(const char *base_name, int flag, size_t max_size, int log_num, int keep_time)
{
    if (base_name == NULL)
        return NULL;

    int use_fork = flag & DLOG_USE_FORK;
    flag &= ~DLOG_USE_FORK;

    int no_cache = flag & DLOG_NO_CACHE;
    flag &= ~DLOG_NO_CACHE;

    int remote_log = flag & DLOG_REMOTE_LOG;
    flag &= ~DLOG_REMOTE_LOG;

    int no_timestamp = flag & DLOG_NO_TIMESTAMP;
    flag &= ~DLOG_NO_TIMESTAMP;

    int log_pid = flag & DLOG_LOG_PID;
    flag &= ~DLOG_LOG_PID;

    dlog_t *log = calloc(1, sizeof(dlog_t));
    if (log == NULL)
        return NULL;

    if (remote_log) {
        log->addr = *((struct sockaddr_in *)base_name);
    } else {
        if ((remote_log = is_remote_log(base_name, &log->addr)) < 0) {
            return dlog_free(log);
        }
        if (remote_log == 0) {
            if ((log->base_name = strdup(base_name)) == NULL) {
                return dlog_free(log);
            }
        }
    }
    if (!remote_log && (flag < DLOG_SHIFT_BY_SIZE || flag > DLOG_SHIFT_BY_DAY))
        return NULL;

    log->name = malloc(strlen(base_name) + 30);
    log->buf_len = WRITE_BUFFER_LEN;
    log->buf = malloc(log->buf_len);
    if (log->name == NULL || log->buf == NULL)
        return dlog_free(log);

    log->shift_type   = flag;
    log->use_fork     = use_fork;
    log->no_cache     = no_cache;
    log->no_timestamp = no_timestamp;
    log->log_pid      = log_pid;
    log->max_size     = max_size;
    log->log_num      = log_num;
    log->keep_time    = keep_time;
    log->remote_log   = remote_log;
    log->sockfd       = -1;

    struct timeval now;
    gettimeofday(&now, NULL);
    log->last_write = now;
    pthread_mutex_init(&log->lock, NULL);

    if (dlog_init_flag == 0) {
        atexit(dlog_atexit);
        dlog_init_flag = 1;
    }

    if (!log->remote_log) {
        int fd = open(get_log_name(log, &now), O_WRONLY | O_APPEND | O_CREAT, 0664);
        if (fd < 0)
            return dlog_free(log);
        close(fd);
    }

    if (log_list_head == NULL) {
        log_list_head = log;
    } else {
        dlog_t *tmp_log = log_list_head;
        while (tmp_log->next)
            tmp_log = (dlog_t *)tmp_log->next;
        tmp_log->next = (void *)log;
    }

    return log;
}

static inline void inner_dlog_check(dlog_t *log, struct timeval *now)
{
    if ((timeval_diff(&log->last_write, now) >= WRITE_INTERVAL_IN_USEC) ||
            (log->write_len >= WRITE_BUFFER_CHECK_LEN)) {
        flush_log(log, now);
    }
}

void dlog_check(dlog_t *log, struct timeval *tv)
{
    struct timeval now;
    if (tv == NULL) {
        gettimeofday(&now, NULL);
        tv = &now;
    }

    if (log) {
        inner_dlog_check(log, tv);
    } else {
        log = log_list_head;
        while (log) {
            if (log->write_len) {
                inner_dlog_check(log, tv);
            }
            log = (dlog_t *)log->next;
        }
    }
}

void dlog_check_all(void)
{
    dlog_check(NULL, NULL);
}

static char *timeval_str(struct timeval *tv)
{
    static char str[64];
    static time_t last_sec;
    if (tv->tv_sec == last_sec) {
        snprintf(str + 20, sizeof(str) - 20, "%06d", (int)tv->tv_usec);
    } else {
        struct tm *t = localtime(&tv->tv_sec);
        snprintf(str, sizeof(str), "%04d-%02d-%02d %02d:%02d:%02d.%06d",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour,
                t->tm_min, t->tm_sec, (int)tv->tv_usec);
        last_sec = tv->tv_sec;
    }

    return str;
}

static int inner_dlog(dlog_t *log, const char *fmt, va_list ap) 
{
    if (!log || !fmt)
        return -1;

    ssize_t ret;
    ssize_t n = 0;
    char *p = log->buf + log->write_len;
    size_t cache_len = log->buf_len - log->write_len;
    char *timestmap;
    struct timeval now;
    gettimeofday(&now, NULL);

    if (!log->no_timestamp) {
        timestmap = timeval_str(&now);
        ret = snprintf(p, cache_len, "[%s] ", timestmap);
        if (ret < 0)
            return -1;
        cache_len -= ret;
        p += ret;
        n += ret;
    }

    if (log->log_pid) {
        ret = snprintf(p, cache_len, "[%d] ", getpid());
        if (ret < 0)
            return -1;
        cache_len -= ret;
        p += ret;
        n += ret;
    }

    va_list cap;
    va_copy(cap, ap);
    ret = vsnprintf(p, cache_len, fmt, cap);
    if (ret < 0) {
        return -1;
    } else if (ret >= cache_len) {
        if (log->remote_log) {
            n += cache_len - 1;
            log->write_len += n;
            flush_log(log, &now);
        } else {
            flush_log(log, &now);
            FILE *fp = fopen(get_log_name(log, &now), "a+");
            if (fp == NULL)
                return -2;
            if (!log->no_timestamp) {
                fprintf(fp, "[%s] ", timestmap);
            }
            if (log->log_pid) {
                fprintf(fp, "[%d] ", getpid());
            }
            vfprintf(fp, fmt, ap);
            fprintf(fp, "\n");
            fclose(fp);
        }
    } else {
        n += ret;
        log->write_len += n;
        log->buf[log->write_len] = '\n';
        log->write_len += 1;

        if (log->no_cache)
            flush_log(log, &now);
        else
            inner_dlog_check(log, &now);
    }

    return 0;
}

int dlog(dlog_t *log, const char *fmt, ...)
{
    pthread_mutex_lock(&log->lock);
    va_list ap;
    va_start(ap, fmt);
    int ret = inner_dlog(log, fmt, ap);
    va_end(ap);
    pthread_mutex_unlock(&log->lock);

    return ret;
}

int dlogv(dlog_t *log, const char *fmt, va_list ap)
{
    pthread_mutex_lock(&log->lock);
    int ret = inner_dlog(log, fmt, ap);
    pthread_mutex_unlock(&log->lock);

    return ret;
}

void dlog_stderr(const char *fmt, ...)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    fprintf(stderr, "[%s] ", timeval_str(&now));

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

void dlog_syslog(const char *fmt, ...)
{
    char exe_path[PATH_MAX];
    ssize_t ret = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    exe_path[ret] = 0;

    openlog(basename(exe_path), LOG_PERROR | LOG_PID, LOG_DAEMON);
    va_list ap;
    va_start(ap, fmt);
    vsyslog(LOG_INFO, fmt, ap);
    va_end(ap);
    closelog();
}

void dlog_backtrace(dlog_t *log)
{
    void *stack[64];
    char **symbols;
    int size, i, j;

    size = backtrace(stack, 64);
    symbols = backtrace_symbols(stack, size);
    if (symbols == NULL)
        return;
    if (size == 1)
        return;

    dlog(log, "%s", "==========backtrace=start==========");
    for (i = 1, j = 0; i < size; ++i, ++j) {
        dlog(log, "%2d %s", j, symbols[i]);
    }
    dlog(log, "%s", "===========backtrace=end===========");
    free(symbols);
}

# ifdef DLOG_SERVER
int dlog_server(dlog_t *log, char *buf, size_t size, struct sockaddr_in *addr)
{
    if (log->remote_log)
        return -1;

    struct timeval now;
    gettimeofday(&now, NULL);

    get_log_name(log, &now);
    shift_log(log, &now);
    unlink_expire(log, &now);

    int fd = open(log->name, O_WRONLY | O_APPEND | O_CREAT, 0664);
    if (fd < 0)
        return -2;

    ssize_t n;
    if (addr) {
        char addr_str[30];
        int len = snprintf(addr_str, sizeof(addr_str), "[%s:%u]\n", \
                inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
        n = write_in_full(fd, addr_str, len);
        if (n < 0) {
            close(fd);
            return -3;
        }
    }
    n = write_in_full(fd, buf, size);
    if (n < 0) {
        close(fd);
        return -4;
    }
    close(fd);

    return 0;
}
# endif

int dlog_fini(dlog_t *log)
{
    if (log == log_list_head) {
        log_list_head = (dlog_t *)log->next;
    } else {
        dlog_t *tmp_log = log_list_head;
        bool not_found = true;
        while (tmp_log) {
            if (tmp_log->next == log) {
                tmp_log->next = log->next;
                not_found = false;
                break;
            } else {
                tmp_log = (dlog_t *)tmp_log->next;
            }
        }
        if (not_found)
            return -1;
    }

    struct timeval now;
    gettimeofday(&now, NULL);

    flush_log(log, &now);
    dlog_free(log);

    return 0;
}

static char *strtolower(char *str)
{
    char *s = str;
    while (*s) {
        if (*s >= 'A' && *s <= 'Z')
            *s += ('a' - 'A');
        ++s;
    }

    return str;
}

int dlog_read_flag(char *str)
{
    if (str == NULL)
        return 0;

    int flag = 0;
    char *s = strdup(str);
    if (s == NULL) 
        return -1;

    char *f = strtok(s, "\r\n\t ,");
    while (f != NULL) {
        strtolower(f);
        if (strcmp(f, "fatal") == 0)
            flag |= DLOG_FATAL;
        else if (strcmp(f, "error") == 0)
            flag |= DLOG_ERROR;
        else if (strcmp(f, "warn") == 0)
            flag |= DLOG_WARN;
        else if (strcmp(f, "info") == 0)
            flag |= DLOG_INFO;
        else if (strcmp(f, "notice") == 0)
            flag |= DLOG_NOTICE;
        else if (strcmp(f, "debug") == 0)
            flag |= DLOG_DEBUG;
        else if (strcmp(f, "trace") == 0)
            flag |= DLOG_TRACE;
        else if (strcmp(f, "user1") == 0)
            flag |= DLOG_USER1;
        else if (strcmp(f, "user2") == 0)
            flag |= DLOG_USER2;
        else
            ;
        f = strtok(NULL, "\r\n\t ,");
    }
    free(s);

    return flag;
}

int dlog_opened_num(void)
{
    int n = 0;
    dlog_t *log = log_list_head;
    while (log) {
        n += 1;
        log = log->next;
    }

    return n;
}

void dlog_set_no_shift(dlog_t *log)
{
    log->no_shift = 1;
}

int dlog_set_sockfd(dlog_t *log, int fd)
{
    if (log->remote_log == 0)
        return -1;

    if (log->sockfd >= 0 && log->external_sockfd == 0) {
        close(log->sockfd);
    }
    log->sockfd = fd;
    log->external_sockfd = 1;

    return 0;
}

void dlog_level_up(void)
{
    int i;
    for (i = 0; i <= 7; ++i) {
        int mask = 0x1 << i;
        if ((default_dlog_flag & mask) == 0) {
            default_dlog_flag |= mask;
            break;
        }
    }
}

void dlog_level_down(void)
{
    int i;
    for (i = 7; i >= 0; --i) {
        int mask = 0x1 << i;
        if (default_dlog_flag & mask) {
            default_dlog_flag &= ~mask;
            break;
        }
    }
}

void dlog_flush(dlog_t *log)
{
    struct timeval now;
    gettimeofday(&now, NULL);

    if (log) {
        flush_log(log, &now);
    } else {
        log = log_list_head;
        while (log) {
            if (log->write_len)
                flush_log(log, &now);
            log = (dlog_t *)log->next;
        }
    }
}

void dlog_flush_all(void)
{
    dlog_flush(NULL);
}

