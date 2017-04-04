/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/28, create
 */

# ifndef _UT_CONFIG_H_
# define _UT_CONFIG_H_

# include <stddef.h>
# include <stdint.h>
# include <stdbool.h>
# include <jansson.h>

# include "nw_svr.h"
# include "nw_clt.h"
# include "ut_cli.h"
# include "ut_log.h"
# include "ut_rpc_clt.h"
# include "ut_rpc_svr.h"

typedef struct process_cfg {
    uint32_t file_limit;
    uint32_t core_limit;
} process_cfg;

typedef struct log_cfg {
    char   *path;
    char   *flag;
    int     shift;
    int     max;
    int     num;
    int     keep;
} log_cfg;

typedef struct inetv4_list {
    size_t count;
    struct sockaddr_in *arr;
} inetv4_list;

typedef struct mysql_cfg {
    char    *host;
    int      port;
    char    *user;
    char    *pass;
    char    *name;
} mysql_cfg;

int parse_inetv4_addr(const char *str, struct sockaddr_in *addr);

int load_cfg_process(json_t *root, const char *key, process_cfg *cfg);
int load_cfg_log(json_t *root, const char *key, log_cfg *cfg);
int load_cfg_svr(json_t *root, const char *key, nw_svr_cfg *cfg);
int load_cfg_clt(json_t *root, const char *key, nw_clt_cfg *cfg);
int load_cfg_rpc_clt(json_t *root, const char *key, rpc_clt_cfg *cfg);
int load_cfg_rpc_svr(json_t *root, const char *key, rpc_svr_cfg *cfg);
int load_cfg_cli_svr(json_t *root, const char *key, cli_svr_cfg *cfg);
int load_cfg_inetv4_list(json_t *root, const char *key, inetv4_list *cfg);
int load_cfg_mysql(json_t *root,  const char *key, mysql_cfg *cfg);

int read_cfg_str(json_t *root, const char *key, char **val, const char *default_val);
int read_cfg_int(json_t *root, const char *key, int *val, bool required, int default_val);
int read_cfg_int32(json_t *root, const char *key, int32_t *val, bool required, int32_t default_val);
int read_cfg_int64(json_t *root, const char *key, int64_t *val, bool required, int64_t default_val);
int read_cfg_uint32(json_t *root, const char *key, uint32_t *val, bool required, uint32_t default_val);
int read_cfg_uint64(json_t *root, const char *key, uint64_t *val, bool required, uint64_t default_val);
int read_cfg_real(json_t *root, const char *key, double *val, bool required, double default_val);
int read_cfg_bool(json_t *root, const char *key, bool *val, bool required, bool default_val);

# endif

