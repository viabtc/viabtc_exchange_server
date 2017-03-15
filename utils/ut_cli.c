/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/31, create
 */

# include <assert.h>
# include <stdlib.h>

# include "ut_cli.h"
# include "ut_log.h"

static int decode_pkg(nw_ses *ses, void *data, size_t max)
{
    char *s = data;
    for (size_t i = 0; i < max; ++i) {
        if (s[i] == '\n')
            return i + 1;
    }
    return 0;
}

static sds on_help(dict_t *dict)
{
    sds result = sdsempty();
    dict_iterator *iter = dict_get_iterator(dict);
    if (iter == NULL)
        return result;
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        result = sdscatprintf(result, "%s\n", (sds)entry->key);
    }
    dict_release_iterator(iter);
    return result;
}

static void on_recv_pkg(nw_ses *ses, void *data, size_t size)
{
    int argc;
    sds s = sdsnewlen(data, size);
    sds *argv = sdssplitargs(s, &argc);
    if (argv == NULL)
        return;
    if (argc == 0) {
        sdsfreesplitres(argv, argc);
        return;
    }

    sds cmd = argv[0];
    sds response = NULL;
    cli_svr *svr = ((nw_svr *)ses->svr)->privdata;
    if (strcmp(cmd, "help") == 0) {
        response = on_help(svr->dict);
    } else {
        dict_entry *entry = dict_find(svr->dict, cmd);
        if (entry) {
            on_cli_cmd callback = entry->val;
            response = callback(argv[0], argc - 1, argv + 1);
        } else {
            response = sdsnew("command not found\n");
        }
    }
    if (response && sdslen(response) > 0) {
        nw_ses_send(ses, response, sdslen(response));
    }
    sdsfreesplitres(argv, argc);
    if (response)
        sdsfree(response);
}

static void on_error_msg(nw_ses *ses, const char *msg)
{
    log_error("peer: %s, error: %s", nw_sock_human_addr(&ses->peer_addr), msg);
}

static uint32_t dict_hash(const void *key)
{
    return dict_generic_hash_function(key, sdslen((sds)key));
}

static void *on_key_dup(const void *key)
{
    return sdsdup((sds)key);
}

static void on_key_destructor(void *key)
{
    sdsfree((sds)key);
}

static int on_key_compare(const void *key1, const void *key2)
{
    return sdscmp((sds)key1, (sds)key2);
}

cli_svr *cli_svr_create(cli_svr_cfg *cfg)
{
    cli_svr *svr = malloc(sizeof(cli_svr));
    assert(svr != NULL);
    
    nw_svr_cfg raw_cfg;
    memset(&raw_cfg, 0, sizeof(raw_cfg));
    raw_cfg.bind_count = 1;
    nw_svr_bind bind;
    raw_cfg.bind_arr = &bind;
    memcpy(&raw_cfg.bind_arr[0].addr, &cfg->addr, sizeof(nw_addr_t));
    raw_cfg.bind_arr[0].sock_type = cfg->sock_type;
    raw_cfg.max_pkg_size = 4096;

    nw_svr_type raw_type;
    memset(&raw_type, 0, sizeof(raw_type));
    raw_type.decode_pkg = decode_pkg;
    raw_type.on_recv_pkg = on_recv_pkg;
    raw_type.on_error_msg = on_error_msg;

    svr->raw_svr = nw_svr_create(&raw_cfg, &raw_type, svr);
    if (svr->raw_svr == NULL) {
        free(svr);
        return NULL;
    }
    nw_svr_start(svr->raw_svr);

    dict_types dict_type;
    memset(&dict_type, 0, sizeof(dict_type));
    dict_type.hash_function = dict_hash;
    dict_type.key_dup = on_key_dup;
    dict_type.key_destructor = on_key_destructor;
    dict_type.key_compare = on_key_compare;
    svr->dict = dict_create(&dict_type, 64);
    if (svr->dict == NULL) {
        nw_svr_release(svr->raw_svr);
        free(svr);
        return NULL;
    }

    return svr;
}

void cli_svr_release(cli_svr *svr)
{
    nw_svr_stop(svr->raw_svr);
    nw_svr_release(svr->raw_svr);
    dict_release(svr->dict);
    free(svr);
}

void cli_svr_add_cmd(cli_svr *svr, const char *cmd, on_cli_cmd callback)
{
    sds key = sdsnew(cmd);
    dict_entry *entry = dict_find(svr->dict, key);
    if (entry) {
        entry->val = callback;
    } else {
        dict_add(svr->dict, key, callback);
    }
}

