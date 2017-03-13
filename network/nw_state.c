/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/22, create
 */

# include <stdlib.h>
# include <stdbool.h>

# include "nw_state.h"

# define NW_STATE_HASH_TABLE_INIT_SIZE 64

nw_state *nw_state_create(nw_state_type *type, uint32_t data_size)
{
    if (type->on_timeout == NULL)
        return NULL;

    nw_loop_init();
    nw_state *context = malloc(sizeof(nw_state));
    if (context == NULL) {
        return NULL;
    }
    memset(context, 0, sizeof(nw_state));
    context->loop = nw_default_loop;
    context->type = *type;
    context->data_size = data_size;
    context->cache = nw_cache_create(sizeof(nw_state_entry) + data_size);
    if (context->cache == NULL) {
        free(context);
        return NULL;
    }
    context->table_size = NW_STATE_HASH_TABLE_INIT_SIZE;
    context->table_mask = context->table_size - 1;
    context->table = calloc(context->table_size, sizeof(nw_state_entry *));
    if (context->table == NULL) {
        nw_cache_release(context->cache);
        free(context);
        return NULL;
    }

    return context;
}

static int expand_if_needed(nw_state *context)
{
    if (context->used < context->table_size * 4)
        return 0;

    uint32_t new_table_size = context->table_size * 4;
    uint32_t new_table_mask = new_table_size - 1;
    nw_state_entry **new_table = calloc(new_table_size, sizeof(nw_state_entry *));
    if (new_table == NULL) {
        return -1;
    }

    for (uint32_t i = 0; i < context->table_size; ++i) {
        nw_state_entry *entry = context->table[i];
        nw_state_entry *next = NULL;
        while (entry) {
            next = entry->next;
            uint32_t index = entry->id & new_table_mask;
            entry->next = new_table[index];
            new_table[index] = entry;
            entry = next;
        }
    }

    free(context->table);
    context->table = new_table;
    context->table_size = new_table_size;
    context->table_mask = new_table_mask;

    return 0;
}

static void state_release(nw_state *context, nw_state_entry *entry)
{
    if (context->type.on_release)
        context->type.on_release(entry);
    nw_cache_free(context->cache, entry);
}

static void state_remove(nw_state *context, nw_state_entry *entry)
{
    uint32_t index = entry->id & context->table_mask;
    nw_state_entry *curr = context->table[index];
    nw_state_entry *prev = NULL;
    while (curr) {
        if (curr->id == entry->id) {
            if (prev) {
                prev->next = entry->next;
            } else {
                context->table[index] = entry->next;
            }
            state_release(context, entry);
            context->used--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

static void on_timeout(struct ev_loop *loop, ev_timer *ev, int events)
{
    nw_state_entry *entry = (nw_state_entry *)ev;
    nw_state *context = entry->context;
    context->type.on_timeout(entry);
    state_remove(context, entry);
}

static uint32_t get_available_id(nw_state *context)
{
    while (true) {
        context->id_start++;
        if (context->id_start == 0)
            context->id_start++;
        if (nw_state_get(context, context->id_start) == NULL)
            break;
    }
    return context->id_start;
}

nw_state_entry *nw_state_add(nw_state *context, double timeout, uint32_t id)
{
    if (id != 0 && nw_state_get(context, id) != NULL)
        return NULL;
    if (context->used == UINT32_MAX)
        return NULL;
    nw_state_entry *entry = nw_cache_alloc(context->cache);
    if (entry == NULL) {
        return NULL;
    }

    if (id != 0) {
        entry->id = id;
    } else {
        entry->id = get_available_id(context);
    }
    ev_timer_init(&entry->ev, on_timeout, timeout, 0);
    ev_timer_start(context->loop, &entry->ev);
    entry->context = context;
    entry->data = ((void *)entry + sizeof(nw_state_entry));
    memset(entry->data, 0, context->data_size);

    expand_if_needed(context);
    uint32_t index = entry->id & context->table_mask;
    entry->next = context->table[index];
    context->table[index] = entry;
    context->used++;

    return entry;
}

nw_state_entry *nw_state_get(nw_state *context, uint32_t id)
{
    uint32_t index = id & context->table_mask;
    nw_state_entry *entry = context->table[index];
    while (entry) {
        if (entry->id == id)
            return entry;
        entry = entry->next;
    }

    return NULL;
}

int nw_state_mod(nw_state *context, uint32_t id, double timeout)
{
    nw_state_entry *entry = nw_state_get(context, id);
    if (entry == NULL)
        return -1;
    ev_timer_stop(context->loop, &entry->ev);
    ev_timer_set(&entry->ev, timeout, 0);
    ev_timer_start(context->loop, &entry->ev);

    return 0;
}

int nw_state_del(nw_state *context, uint32_t id)
{
    nw_state_entry *entry = nw_state_get(context, id);
    if (entry == NULL)
        return -1;
    ev_timer_stop(context->loop, &entry->ev);
    state_remove(context, entry);

    return 0;
}

size_t nw_state_count(nw_state *context)
{
    return context->used;
}

void nw_state_release(nw_state *context)
{
    for (uint32_t i = 0; i < context->table_size; ++i) {
        nw_state_entry *entry = context->table[i];
        nw_state_entry *next = NULL;
        while (entry) {
            next = entry->next;
            ev_timer_stop(context->loop, &entry->ev);
            state_release(context, entry);
            entry = next;
        }
    }
    nw_cache_release(context->cache);
    free(context->table);
    free(context);
}

nw_state_iterator *nw_state_get_iterator(nw_state *context)
{
    nw_state_iterator *iter = malloc(sizeof(nw_state_iterator));
    if (iter == NULL)
        return NULL;
    memset(iter, 0, sizeof(nw_state_iterator));
    iter->context = context;
    iter->index = -1;
    iter->entry = NULL;
    iter->next_entry = NULL;

    return iter;
}

nw_state_entry *nw_state_next(nw_state_iterator *iter)
{
    while (true) {
        if (iter->entry == NULL) {
            iter->index++;
            if (iter->index >= iter->context->table_size)
                break;
            iter->entry = iter->context->table[iter->index];
        } else {
            iter->entry = iter->next_entry;
        }
        if (iter->entry) {
            iter->next_entry = iter->entry->next;
            return iter->entry;
        }
    }

    return NULL;
}

void nw_state_iterator_release(nw_state_iterator *iter)
{
    free(iter);
}

