/*
 * Description: Hash table implementation.
 *     History: yang@haipo.me, 2016/03/16, create
 */

# ifndef _DICT_H_
# define _DICT_H_

# include <stdint.h>
# include <stddef.h>

typedef struct dict_entry {
    void *key;
    void *val;
    uint64_t id;
    struct dict_entry *next;
} dict_entry;

typedef struct dict_types {
    uint32_t (*hash_function)(const void *key);
    void *(*key_dup)(const void *key);
    void *(*val_dup)(const void *val);
    int (*key_compare)(const void *key1, const void *key2);
    void (*key_destructor)(void *key);
    void (*val_destructor)(void *val);
} dict_types;

typedef struct dict_t {
    dict_entry **table;
    dict_types type;
    uint32_t size;
    uint32_t mask;
    uint32_t used;
    uint64_t id_start;
    uint64_t id_clear;
} dict_t;

typedef struct dict_iterator {
    dict_t *dt;
    int64_t index;
    dict_entry *entry;
    dict_entry *next_entry;
} dict_iterator;

# define dict_size(dt) (dt)->used
# define dict_slot(dt) (dt)->size

uint32_t dict_generic_hash_function(const void *data, size_t len);

dict_t *dict_create(dict_types *type, uint32_t init_size);
dict_entry *dict_add(dict_t *dt, void *key, void *val);
dict_entry *dict_find(dict_t *dt, const void *key);
int dict_expand(dict_t *dt, uint32_t size);
int dict_replace(dict_t *dt, void *key, void *val);
int dict_delete(dict_t *dt, const void *key);
void dict_release(dict_t *dt);

void dict_clear(dict_t *dt);
void dict_mark_clear(dict_t *dt);

dict_iterator *dict_get_iterator(dict_t *dt);
dict_entry *dict_next(dict_iterator *iter);
void dict_release_iterator(dict_iterator *iter);

# endif

