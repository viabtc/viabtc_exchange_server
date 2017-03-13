/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/22, create
 */

# ifndef _NW_STATE_H_
# define _NW_STATE_H_

# include <stdint.h>
# include <stddef.h>

# include "nw_evt.h"
# include "nw_buf.h"

/* nw_state is a state machine with timeout */

typedef struct nw_state_entry {
    ev_timer ev;
    /* state id */
    uint32_t id;
    /* state context, the nw_state instance */
    void *context;
    /* state data, user define */
    void *data;
    struct nw_state_entry *next;
} nw_state_entry;

typedef struct nw_state_type {
    /* must
     *
     * called when a state is timeout, after call, the state will be automatic deleted */
    void (*on_timeout)(nw_state_entry *entry);
    /* optional
     *
     * called when a state is deleted */
    void (*on_release)(nw_state_entry *entry);
} nw_state_type;

typedef struct nw_state {
    struct ev_loop *loop;
    nw_state_type type;
    uint32_t data_size;
    nw_cache *cache;
    nw_state_entry **table;
    uint32_t table_size;
    uint32_t table_mask;
    uint32_t used;
    uint32_t id_start;
} nw_state;

typedef struct nw_state_iterator {
    nw_state *context;
    int64_t index;
    nw_state_entry *entry;
    nw_state_entry *next_entry;
} nw_state_iterator;

/* create an state machine instance, with the fixed user data size */
nw_state *nw_state_create(nw_state_type *type, uint32_t data_size);
nw_state_entry *nw_state_add(nw_state *context, double timeout, uint32_t id);
nw_state_entry *nw_state_get(nw_state *context, uint32_t id);
int nw_state_mod(nw_state *context, uint32_t id, double timeout);
int nw_state_del(nw_state *context, uint32_t id);
/* return the total pending state count in a state machine */
size_t nw_state_count(nw_state *context);
void nw_state_release(nw_state *context);

/* traverse the state machine */
nw_state_iterator *nw_state_get_iterator(nw_state *context);
nw_state_entry *nw_state_next(nw_state_iterator *iter);
void nw_state_iterator_release(nw_state_iterator *iter);

# endif

