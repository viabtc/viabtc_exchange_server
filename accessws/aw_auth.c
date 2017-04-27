/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/27, create
 */

# include <curl/curl.h>

# include "aw_config.h"
# include "aw_server.h"
# include "aw_auth.h"

static nw_job *job_context;
static nw_state *state_context;

struct state_data {
    nw_ses *ses;
    uint64_t ses_id;
    uint64_t request_id;
    struct clt_info *info;
};

static size_t post_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    sds *reply = userdata;
    *reply = sdscatlen(*reply, ptr, size * nmemb);
    return size * nmemb;
}

static void on_job(nw_job_entry *entry, void *privdata)
{
    CURL *curl = curl_easy_init();
    sds reply = sdsempty();
    sds token = sdsempty();
    struct curl_slist *chunk = NULL;
    token = sdscatprintf(token, "Authorization: %s", (sds)entry->request);
    chunk = curl_slist_append(chunk, token);
    chunk = curl_slist_append(chunk, "Accept-Language: en_US");
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_URL, settings.auth_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, post_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &reply);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, (long)(settings.timeout * 1000));

    CURLcode ret = curl_easy_perform(curl);
    if (ret != CURLE_OK) {
        log_error("curl_easy_perform fail: %s", curl_easy_strerror(ret));
        goto cleanup;
    }

    log_trace("reply: %s", reply);
    json_t *result = json_loads(reply, 0, NULL);
    if (result == NULL)
        goto cleanup;
    entry->reply = result;
    return;

cleanup:
    curl_easy_cleanup(curl);
    sdsfree(reply);
    sdsfree(token);
    curl_slist_free_all(chunk);
}

static void on_finish(nw_job_entry *entry)
{
    nw_state_entry *r = nw_state_get(state_context, entry->id);
    if (r == NULL)
        return;
    struct state_data *state = r->data;
    sds token = entry->request;
    json_t *result = entry->reply;
    if (result == NULL)
        goto fail;
    int code = json_integer_value(json_object_get(result, "code"));
    if (code != 0) {
        const char *message = json_string_value(json_object_get(result, "message"));
        log_error("auth fail, token: %s, code: %d, message: %s", token, code, message ? message : "");
        goto fail;
    }
    json_t *data = json_object_get(result, "data");
    if (data == NULL)
        goto fail;
    struct clt_info *info = state->info;
    info->user_id = json_integer_value(json_object_get(data, "user_id"));
    info->taker_fee = decimal(json_string_value(json_object_get(data, "taker_fee_rate")), 0);
    info->maker_fee = decimal(json_string_value(json_object_get(data, "maker_fee_rate")), 0);
    if (info->user_id == 0 || info->taker_fee == NULL || info->maker_fee == NULL)
        goto fail;

    if (state->ses->id == state->ses_id)
        send_success(state->ses, state->request_id);
    nw_state_del(state_context, entry->id);
    return;

fail:
    if (state->ses->id == state->ses_id)
        send_error(state->ses, state->request_id, 10, "auth fail");
    nw_state_del(state_context, entry->id);
}

static void on_cleanup(nw_job_entry *entry)
{
    sdsfree(entry->request);
    if (entry->reply)
        json_decref(entry->reply);
}

static void on_timeout(nw_state_entry *entry)
{
    struct state_data *state = entry->data;
    send_error_service_timeout(state->ses, state->request_id);
}

int on_server_auth(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (json_array_size(params) != 1)
        return send_error_invalid_argument(ses, id);
    const char *token = json_string_value(json_array_get(params, 0));
    if (token == NULL)
        return send_error_invalid_argument(ses, id);

    nw_state_entry *entry = nw_state_add(state_context, settings.timeout, 0);
    struct state_data *state = entry->data;
    state->ses = ses;
    state->ses_id = ses->id;
    state->request_id = id;
    state->info = info;

    nw_job_add(job_context, entry->id, sdsnew(token));
    return 0;
}

int init_auth(void)
{

    nw_job_type jt;
    memset(&jt, 0, sizeof(jt));
    jt.on_job = on_job;
    jt.on_finish = on_finish;
    jt.on_cleanup = on_cleanup;

    job_context = nw_job_create(&jt, 10);
    if (job_context == NULL)
        return -__LINE__;

    nw_state_type st;
    memset(&st, 0, sizeof(st));
    st.on_timeout = on_timeout;

    state_context = nw_state_create(&st, sizeof(struct state_data));
    if (state_context == NULL)
        return -__LINE__;

    return 0;
}

