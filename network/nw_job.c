/*
 * Description: 
 *     History: yang@haipo.me, 2016/04/17, create
 */

# include <stdlib.h>
# include <unistd.h>
# include <assert.h>

# include "nw_job.h"
# include "nw_sock.h"

struct thread_arg {
    nw_job *job;
    void *privdata;
};

static void *thread_routine(void *data)
{
    struct thread_arg *arg = data;
    nw_job *job = arg->job;
    void *privdata = arg->privdata;
    free(data);

    for (;;) {
        pthread_mutex_lock(&job->lock);
        while ((job->request_count == 0) && (!job->shutdown)) {
            pthread_cond_wait(&job->notify, &job->lock);
        }
        if (job->shutdown) {
            break;
        }
        assert(job->request_head != NULL);
        nw_job_entry *entry = job->request_head;
        job->request_head = entry->next;
        if (job->request_head) {
            job->request_head->prev = NULL;
        } else {
            job->request_tail = NULL;
        }
        job->request_count -= 1;
        pthread_mutex_unlock(&job->lock);

        job->type.on_job(entry, privdata);

        pthread_mutex_lock(&job->lock);
        if (job->reply_tail) {
            entry->prev = job->reply_tail;
            entry->next = NULL;
            job->reply_tail->next = entry;
            job->reply_tail = entry;
        } else {
            entry->prev = NULL;
            entry->next = NULL;
            job->reply_head = entry;
            job->reply_tail = entry;
        }
        job->reply_count += 1;
        write(job->pipefd[1], " ", 1);
        pthread_mutex_unlock(&job->lock);
    }

    pthread_mutex_unlock(&job->lock);
    return privdata;
}

static void on_can_read(struct ev_loop *loop, ev_io *watcher, int events)
{
    nw_job *job = (nw_job *)watcher;
    for (;;) {
        char c;
        int ret = read(job->pipefd[0], &c, 1);
        if (ret < 0)
            break;
    }

    for (;;) {
        pthread_mutex_lock(&job->lock);
        if (job->reply_count == 0) {
            pthread_mutex_unlock(&job->lock);
            break;
        }
        nw_job_entry *entry = job->reply_head;
        job->reply_head = entry->next;
        if (job->reply_head) {
            job->reply_head->prev = NULL;
        } else {
            job->reply_tail = NULL;
        }
        job->reply_count -= 1;
        pthread_mutex_unlock(&job->lock);

        if (job->type.on_finish)
            job->type.on_finish(entry);
        if (job->type.on_cleanup)
            job->type.on_cleanup(entry);
        nw_cache_free(job->cache, entry);
    }
}

static void nw_job_free(nw_job *job)
{
    pthread_mutex_destroy(&job->lock);
    pthread_cond_destroy(&job->notify);
    if (job->threads)
        free(job->threads);
    free(job);
}

nw_job *nw_job_create(nw_job_type *type, int thread_count)
{
    if (!type->on_job)
        return NULL;
    if (type->on_init && !type->on_release)
        return NULL;

    nw_job *job = malloc(sizeof(nw_job));
    if (job == NULL)
        return NULL;
    memset(job, 0, sizeof(nw_job));
    nw_loop_init();
    job->type = *type;
    job->loop = nw_default_loop;
    if (pthread_mutex_init(&job->lock, NULL) != 0) {
        free(job);
        return NULL;
    }
    if (pthread_cond_init(&job->notify, NULL) != 0) {
        pthread_mutex_destroy(&job->lock);
        free(job);
        return NULL;
    }
    job->thread_count = thread_count;
    job->threads = calloc(job->thread_count, sizeof(pthread_t));
    if (job->threads == NULL) {
        nw_job_free(job);
        return NULL;
    }
    job->cache = nw_cache_create(sizeof(nw_job_entry));
    if (job->cache == NULL) {
        nw_job_free(job);
        return NULL;
    }
    if (pipe(job->pipefd) != 0) {
        nw_job_free(job);
        return NULL;
    }
    nw_sock_set_nonblock(job->pipefd[0]);
    nw_sock_set_nonblock(job->pipefd[1]);
    ev_io_init(&job->ev, on_can_read, job->pipefd[0], EV_READ);
    ev_io_start(job->loop, &job->ev);

    for (int i = 0; i < job->thread_count; ++i) {
        struct thread_arg *arg = malloc(sizeof(struct thread_arg));
        if (arg == NULL) {
            nw_job_release(job);
            return NULL;
        }
        memset(arg, 0, sizeof(struct thread_arg));
        arg->job = job;
        if (job->type.on_init) {
            arg->privdata = job->type.on_init();
            if (arg->privdata == NULL) {
                nw_job_release(job);
                return NULL;
            }
        }
        if (pthread_create(&job->threads[i], NULL, thread_routine, arg) != 0) {
            nw_job_release(job);
            return NULL;
        }
        job->thread_start++;
    }

    return job;
}

int nw_job_add(nw_job *job, uint32_t id, void *request)
{
    nw_job_entry *entry = nw_cache_alloc(job->cache);
    if (entry == NULL)
        return -1;
    memset(entry, 0, sizeof(nw_job_entry));
    entry->id = id;
    entry->request = request;

    pthread_mutex_lock(&job->lock);
    if (job->request_tail) {
        entry->prev = job->request_tail;
        entry->next = NULL;
        job->request_tail->next = entry;
        job->request_tail = entry;
    } else {
        entry->prev = NULL;
        entry->next = NULL;
        job->request_head = entry;
        job->request_tail = entry;
    }
    job->request_count += 1;
    pthread_cond_signal(&job->notify);
    pthread_mutex_unlock(&job->lock);

    return 0;
}

void nw_job_release(nw_job *job)
{
    pthread_mutex_lock(&job->lock);
    if (job->shutdown) {
        pthread_mutex_unlock(&job->lock);
        return;
    }
    job->shutdown = true;
    pthread_cond_broadcast(&job->notify);
    pthread_mutex_unlock(&job->lock);
    for (int i = 0; i < job->thread_start; ++i) {
        void *privdata = NULL;
        if (pthread_join(job->threads[i], &privdata) != 0) {
            continue;
        }
        if (privdata != NULL && privdata != PTHREAD_CANCELED) {
            job->type.on_release(privdata);
        }
    }
    ev_io_stop(job->loop, &job->ev);
    close(job->pipefd[0]);
    close(job->pipefd[1]);
    nw_job_free(job);
}

