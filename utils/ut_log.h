/*
 * Description: a log lib with cache, writen by damon
 *     History: yang@haipo.me, 2013/05/20, created
 */

# ifndef _DLOG_H_
# define _DLOG_H_

# define __DLOG__ 1

# include <stddef.h>
# include <stdarg.h>
# include <pthread.h>
# include <time.h>
# include <sys/time.h>
# include <netinet/in.h>

/*
 * To use this log library, you have two more things need to do.
 *
 * 1. catch the SIGSEGV signal with flag SA_RESETHAND, and call dlog_flush_all
 *    in the signal process function to make sure log buffer are write into file.
 *
 * 2. call dlog_check_all in the main loop or in timer, to help write log to file
 *    as soon as possible.
 */

enum {
    DLOG_SHIFT_BY_SIZE = 1,
    DLOG_SHIFT_BY_MIN,
    DLOG_SHIFT_BY_HOUR,
    DLOG_SHIFT_BY_DAY,
};

typedef struct {
    char                *base_name;
    char                *name;
    struct timeval      last_write;
    char                *buf;
    size_t              buf_len;
    size_t              write_len;
    int                 no_shift;
    int                 shift_type;
    time_t              last_shift;
    int                 use_fork;
    int                 no_cache;
    int                 no_timestamp;
    int                 log_pid;
    size_t              max_size;
    int                 log_num;
    int                 keep_time;
    time_t              last_unlink;
    int                 remote_log;
    int                 sockfd;
    int                 external_sockfd;
    struct sockaddr_in  addr;
    pthread_mutex_t     lock;
    void                *next;
} dlog_t;


/* flags  */

/* when remove expire log file or shift log file, fork a new process to do those job. */
# define DLOG_USE_FORK      0x10000

/* use DLOG_NO_CACHE with flag, log will write to file immediately. */
# define DLOG_NO_CACHE      0x20000

/* also set base_name as a pointer to struct sockaddr_in, log will be send to remote addr. */
# define DLOG_REMOTE_LOG    0x40000

/* don't prepend timestamp to every message */
# define DLOG_NO_TIMESTAMP  0x80000

/* log pid */
# define DLOG_LOG_PID       0x100000

/*
 * example:
 * dlog_init("test", DLOG_SHIFT_BY_DAY | DLOG_USE_FORK, 1000 * 1000 * 1000, 0, 30);
 * this create a dlog_t handler, which log file name start with test, max size of per
 * log file is 1G, use new process shift and remove log files, keep log files 30 days.
 *
 * fail return NULL.
 */
dlog_t *dlog_init(const char *base_name, int flag, size_t max_size, int log_num, int keep_time);

/* 
 * set udp socket file descriptor when use remote log.
 * this is optional, if you don't set, dlog will create one.
 */
int dlog_set_sockfd(dlog_t *log, int fd);

/* log a message */
int dlog(dlog_t *log, const char *fmt, ...) __attribute__ ((format(printf, 2, 3)));
int dlogv(dlog_t *log, const char *fmt, va_list ap);

/* log to stderr */
void dlog_stderr(const char *fmt, ...);

/* log to syslog */
void dlog_syslog(const char *fmt, ...);

/* log stack backtrace */
void dlog_backtrace(dlog_t *log);

/* check log, helog write log to file as soon as possible */
void dlog_check(dlog_t *log, struct timeval *tv);
void dlog_check_all(void);

/* flush log immediately */
void dlog_flush(dlog_t *log);
void dlog_flush_all(void);

/* close a log object */
int dlog_fini(dlog_t *log);

# ifdef DLOG_SERVER
int dlog_server(dlog_t *log, char *buf, size_t size, struct sockaddr_in *addr);
# endif

/* return all opened dlog instance number */
int dlog_opened_num(void);

/* set no shift log */
void dlog_set_no_shift(dlog_t *log);

/*
 * default_dlog and default_dlog_flag is use for macros below.
 * default_dlog_flag is used in bits, you need initialize it like:
 *     default_dlog_flag = DLOG_FATAL | DLOG_ERROR | DLOG_INFO;
 */
extern dlog_t   *default_dlog;
extern int       default_dlog_flag;

typedef int (*dlog_on_fatal_cb)(const char *fmt, ...);
extern dlog_on_fatal_cb dlog_on_fatal;

/* read flag from string like: error, info, NOTICE  DEBUG */
int dlog_read_flag(char *str);

void dlog_level_up(void);
void dlog_level_down(void);

enum {
    DLOG_FATAL  = 0x1,
    DLOG_ERROR  = 0x2,
    DLOG_WARN   = 0x4,
    DLOG_INFO   = 0x8,
    DLOG_NOTICE = 0x10,
    DLOG_DEBUG  = 0x20,
    DLOG_TRACE  = 0x40,
    DLOG_USER1  = 0x80,
    DLOG_USER2  = 0x100,
};

# define log_stderr(fmt, args...) dlog_stderr("%s:%i(%s): " fmt, __FILE__, __LINE__, __func__, ##args);

# define loga(fmt, args...) do { \
    if (default_dlog) { \
        dlog(default_dlog, fmt, ##args); \
    } else { \
        dlog_syslog(fmt, ##args); \
    } \
} while (0)

# define log_vip(fmt, args...) do { \
    loga("[vip]%s:%i(%s): " fmt, __FILE__, __LINE__, __func__, ##args); \
} while (0)

# define log_fatal(fmt, args...) do { \
    if (default_dlog_flag & DLOG_FATAL) { \
        loga("[fatal]%s:%i(%s): " fmt, __FILE__, __LINE__, __func__, ##args); \
        if (dlog_on_fatal) { \
            dlog_on_fatal("[fatal]%s:%i(%s): " fmt, __FILE__, __LINE__, __func__, ##args); \
        } \
    } \
} while (0)

# define log_error(fmt, args...) do { \
    if (default_dlog_flag & DLOG_ERROR) { \
        loga("[error]%s:%i(%s): " fmt, __FILE__, __LINE__, __func__, ##args); \
    } \
} while (0)

# define log_warn(fmt, args...) do { \
    if (default_dlog_flag & DLOG_WARN) { \
        loga("[warn]%s:%i(%s): " fmt, __FILE__, __LINE__, __func__, ##args); \
    } \
} while (0)

# define log_info(fmt, args...) do { \
    if (default_dlog_flag & DLOG_INFO) { \
        loga("[info]%s:%i(%s): " fmt, __FILE__, __LINE__, __func__, ##args); \
    } \
} while (0)

# define log_notice(fmt, args...) do { \
    if (default_dlog_flag & DLOG_NOTICE) { \
        loga("[notice]%s:%i(%s): " fmt, __FILE__, __LINE__, __func__, ##args); \
    } \
} while (0)

# define log_debug(fmt, args...) do { \
    if (default_dlog_flag & DLOG_DEBUG) { \
        loga("[debug]%s:%i(%s): " fmt, __FILE__, __LINE__, __func__, ##args); \
    } \
} while (0)

# define log_trace(fmt, args...) do { \
    if (default_dlog_flag & DLOG_TRACE) { \
        loga("[trace]%s:%i(%s): " fmt, __FILE__, __LINE__, __func__, ##args); \
    } \
} while (0)

# define log_user1(fmt, args...) do { \
    if (default_dlog_flag & DLOG_USER1) { \
        loga("[user1]%s:%i(%s): " fmt, __FILE__, __LINE__, __func__, ##args); \
    } \
} while (0)

# define log_user2(fmt, args...) do { \
    if (default_dlog_flag & DLOG_USER2) { \
        loga("[user2]%s:%i(%s): " fmt, __FILE__, __LINE__, __func__, ##args); \
    } \
} while (0)

# define log_exception(fmt, args...) do { \
    if (default_dlog_flag & DLOG_FATAL) { \
        loga("[fatal]%s:%i(%s): " fmt , __FILE__, __LINE__, __func__, ##args); \
        if (default_dlog) { \
            dlog_backtrace(default_dlog); \
        } \
    } \
} while (0)

# endif

