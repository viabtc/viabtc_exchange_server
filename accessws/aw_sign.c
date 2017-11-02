/*
 * Description: 
 *     History: yang@haipo.me, 2017/11/02, create
 */

# include <curl/curl.h>

# include "aw_config.h"
# include "aw_server.h"
# include "aw_asset.h"
# include "aw_order.h"
# include "aw_sign.h"

static nw_job *job_context;
static nw_state *state_context;

struct sign_request {
    sds access_id;
    sds authorisation;
    uint64_t tonce;
};

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
    struct sign_request *request = entry->request;

    sds reply = sdsempty();
    sds token = sdsempty();
    sds url   = sdsempty();
    struct curl_slist *chunk = NULL;

    char *access_id = curl_easy_escape(curl, request->access_id, 0);
    url = sdscatprintf(url, "%s?access_id=%s&tonce=%"PRIu64, settings.sign_url, access_id, request->tonce);
    free(access_id);

    token = sdscatprintf(token, "Authorization: %s", request->authorisation);
    chunk = curl_slist_append(chunk, token);
    chunk = curl_slist_append(chunk, "Accept-Language: en_US");
    chunk = curl_slist_append(chunk, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, post_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &reply);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, (long)(settings.backend_timeout * 1000));

    CURLcode ret = curl_easy_perform(curl);
    if (ret != CURLE_OK) {
        log_fatal("curl_easy_perform fail: %s", curl_easy_strerror(ret));
        goto cleanup;
    }

    json_t *result = json_loads(reply, 0, NULL);
    if (result == NULL)
        goto cleanup;
    entry->reply = result;

cleanup:
    curl_easy_cleanup(curl);
    sdsfree(reply);
    sdsfree(token);
    sdsfree(url);
    curl_slist_free_all(chunk);
}

static void on_result(struct state_data *state, struct sign_request *request, json_t *result)
{
    if (state->ses->id != state->ses_id)
        return;
    if (result == NULL)
        goto error;

    json_t *code = json_object_get(result, "code");
    if (code == NULL)
        goto error;
    int error_code = json_integer_value(code);
    if (error_code != 0) {
        const char *message = json_string_value(json_object_get(result, "message"));
        if (message == NULL)
            goto error;
        log_error("sign fail, access_id: %s, authorisation: %s, token: %"PRIu64" code: %d, message: %s",
                request->access_id, request->authorisation, request->tonce, error_code, message);
        send_error(state->ses, state->request_id, 11, message);
        return;
    }

    json_t *data = json_object_get(result, "data");
    if (data == NULL)
        goto error;
    struct clt_info *info = state->info;
    uint32_t user_id = json_integer_value(json_object_get(data, "user_id"));
    if (user_id == 0)
        goto error;

    if (info->auth && info->user_id != user_id) {
        asset_unsubscribe(info->user_id, state->ses);
        order_unsubscribe(info->user_id, state->ses);
    }

    info->auth = true;
    info->user_id = user_id;
    log_error("sign success, access_id: %s, user_id: %u", request->access_id, user_id);
    send_success(state->ses, state->request_id);

    return;

error:
    if (result) {
        char *reply = json_dumps(result, 0);
        log_fatal("invalid reply: %s", reply);
        free(reply);
    }
    send_error_internal_error(state->ses, state->request_id);
}

static void on_finish(nw_job_entry *entry)
{
    nw_state_entry *state = nw_state_get(state_context, entry->id);
    if (state == NULL)
        return;
    on_result(state->data, entry->request, entry->reply);
    nw_state_del(state_context, entry->id);
}

static void on_cleanup(nw_job_entry *entry)
{
    struct sign_request *request = entry->request;
    sdsfree(request->access_id);
    sdsfree(request->authorisation);
    free(request);
    if (entry->reply)
        json_decref(entry->reply);
}

static void on_timeout(nw_state_entry *entry)
{
    struct state_data *state = entry->data;
    if (state->ses->id == state->ses_id) {
        send_error_service_timeout(state->ses, state->request_id);
    }
}

int send_sign_request(nw_ses *ses, uint64_t id, struct clt_info *info, json_t *params)
{
    if (json_array_size(params) != 3)
        return send_error_invalid_argument(ses, id);
    const char *access_id = json_string_value(json_array_get(params, 0));
    if (access_id == NULL)
        return send_error_invalid_argument(ses, id);
    const char *authorisation = json_string_value(json_array_get(params, 1));
    if (authorisation == NULL)
        return send_error_invalid_argument(ses, id);
    uint64_t tonce = json_integer_value(json_array_get(params, 2));
    if (tonce == 0)
        return send_error_invalid_argument(ses, id);

    nw_state_entry *entry = nw_state_add(state_context, settings.backend_timeout, 0);
    struct state_data *state = entry->data;
    state->ses = ses;
    state->ses_id = ses->id;
    state->request_id = id;
    state->info = info;

    log_info("send sign request, access_id: %s, authorisation: %s, tonce: %"PRIu64, access_id, authorisation, tonce);
    info->source = strdup("api");

    struct sign_request *request = malloc(sizeof(struct sign_request));
    request->access_id = sdsnew(access_id);
    request->authorisation = sdsnew(authorisation);
    request->tonce = tonce;
    nw_job_add(job_context, entry->id, request);

    return 0;
}

int init_sign(void)
{
    nw_job_type jt;
    memset(&jt, 0, sizeof(jt));
    jt.on_job = on_job;
    jt.on_finish = on_finish;
    jt.on_cleanup = on_cleanup;

    job_context = nw_job_create(&jt, 5);
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

