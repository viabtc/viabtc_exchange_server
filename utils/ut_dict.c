/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/16, create
 */

# include <stdlib.h>
# include <string.h>
# include "ut_dict.h"

/* fnv hash */
uint32_t dict_generic_hash_function(const void *data, size_t len)
{
    uint32_t hval = 0x811c9dc5;
    unsigned char *bp = (unsigned char *)data;
    unsigned char *be = bp + len;

    while (bp < be) {
        hval ^= (uint32_t)*bp++;
        hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) + (hval << 24);
    }

    return hval;
}

# define DICT_SET_HASH_KEY(dt, entry, key) do { \
    if ((dt)->type.key_dup) { \
        (entry)->key = (dt)->type.key_dup(key); \
    } else { \
        (entry)->key = key; \
    } \
} while (0)

# define DICT_FREE_HASH_KEY(dt, entry) do { \
    if ((dt)->type.key_destructor) { \
        (dt)->type.key_destructor((entry)->key); \
    } \
} while (0)

# define DICT_SET_HASH_VAL(dt, entry, val) do { \
    if ((dt)->type.val_dup) { \
        (entry)->val = (dt)->type.val_dup(val); \
    } else { \
        (entry)->val = val; \
    } \
} while (0)

# define DICT_FREE_HASH_VAL(dt, entry) do { \
    if ((dt)->type.val_destructor) { \
        (dt)->type.val_destructor((entry)->val); \
    } \
} while (0)

# define DICT_HASH_KEY(dt, key) (dt)->type.hash_function(key)
# define DICT_COMPARE_KEY(dt, key1, key2) (dt)->type.key_compare((key1), (key2))

static uint32_t dict_next_power(uint32_t size)
{
    uint32_t realsize = 4;
    while (1) {
        if (realsize >= size)
            return realsize;
        realsize *= 2;
    }
}

dict_t *dict_create(dict_types *type, uint32_t init_size)
{
    if (type->hash_function == NULL)
        return NULL;
    if (type->key_compare == NULL)
        return NULL;
    dict_t *dt = malloc(sizeof(dict_t));
    if (dt == NULL)
        return NULL;
    memset(dt, 0, sizeof(dict_t));
    memcpy(&dt->type, type, sizeof(dict_types));
    dt->size = dict_next_power(init_size);
    dt->mask = dt->size - 1;
    dt->used = 0;
    dt->table = calloc(dt->size, sizeof(dict_entry *));
    if (dt->table == NULL) {
        free(dt);
        return NULL;
    }

    return dt;
}

int dict_expand(dict_t *dt, uint32_t size)
{
    dict_t new;
    memcpy(&new, dt, sizeof(dict_t));
    new.size = dict_next_power(size);
    new.mask = new.size - 1;
    new.table = calloc(new.size, sizeof(dict_entry *));
    if (new.table == NULL)
        return -1;

    for (uint32_t i = 0; i < dt->size && dt->used > 0; ++i) {
        dict_entry *entry = dt->table[i];
        dict_entry *next_entry = NULL;
        while (entry) {
            next_entry = entry->next;
            uint32_t index = DICT_HASH_KEY(dt, entry->key) & new.mask;
            entry->next = new.table[index];
            new.table[index] = entry;
            entry = next_entry;
            dt->used--;
        }
    }

    free(dt->table);
    memcpy(dt, &new, sizeof(dict_t));

    return 0;
}

static int dict_expand_if_needed(dict_t *dt)
{
    if (dt->used >= dt->size * 4)
        return dict_expand(dt, dt->size * 4);
    return 0;
}

static void check_clear(dict_t *dt, uint32_t index)
{
    dict_entry *entry = dt->table[index];
    dict_entry *prev = NULL;
    dict_entry *next = NULL;
    
    while (entry) {
        next = entry->next;
        if (entry->id < dt->id_clear) {
            if (prev) {
                prev->next = next;
            } else {
                dt->table[index] = next;
            }
            DICT_FREE_HASH_KEY(dt, entry);
            DICT_FREE_HASH_VAL(dt, entry);
            free(entry);
            entry = NULL;
            dt->used--;
        }
        if (entry)
            prev = entry;
        entry = next;
    }
}

dict_entry *dict_find(dict_t *dt, const void *key)
{
    uint32_t index = DICT_HASH_KEY(dt, key) & dt->mask;
    if (dt->id_clear > 0) {
        check_clear(dt, index);
    }
    dict_entry *entry = dt->table[index];
    while (entry) {
        if (DICT_COMPARE_KEY(dt, key, entry->key) == 0)
            return entry;
        entry = entry->next;
    }
    return NULL;
}

dict_entry *dict_add(dict_t *dt, void *key, void *val)
{
    if (dict_find(dt, key) != NULL)
        return NULL;
    if (dict_expand_if_needed(dt) != 0)
        return NULL;
    dict_entry *entry = malloc(sizeof(dict_entry));
    if (entry == NULL)
        return NULL;

    uint32_t index = DICT_HASH_KEY(dt, key) & dt->mask;
    entry->id = dt->id_start++;
    entry->next = dt->table[index];
    dt->table[index] = entry;
    DICT_SET_HASH_KEY(dt, entry, key);
    DICT_SET_HASH_VAL(dt, entry, val);
    dt->used++;

    return entry;
}

int dict_replace(dict_t *dt, void *key, void *val)
{
    dict_entry *entry = dict_find(dt, key);
    if (entry == NULL) {
        if (dict_add(dt, key, val) == NULL)
            return -1;
        return 1;
    }
    dict_entry old_entry;
    memcpy(&old_entry, entry, sizeof(dict_entry));
    DICT_SET_HASH_VAL(dt, entry, val);
    DICT_FREE_HASH_VAL(dt, &old_entry);

    return 0;
}

int dict_delete(dict_t *dt, const void *key)
{
    uint32_t index = DICT_HASH_KEY(dt, key) & dt->mask;
    dict_entry *entry = dt->table[index];
    dict_entry *prev = NULL;
    
    while (entry) {
        if (DICT_COMPARE_KEY(dt, entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                dt->table[index] = entry->next;
            }
            DICT_FREE_HASH_KEY(dt, entry);
            DICT_FREE_HASH_VAL(dt, entry);
            free(entry);
            dt->used--;
            return 1;
        }
        prev = entry;
        entry = entry->next;
    }

    return 0;
}

void dict_clear(dict_t *dt)
{
    dict_iterator *iter = dict_get_iterator(dt);
    if (iter == NULL)
        return;
    dict_entry *entry;
    while ((entry = dict_next(iter)) != NULL) {
        dict_delete(dt, entry->key);
    }
    dict_release_iterator(iter);
}

void dict_mark_clear(dict_t *dt)
{
    dt->id_clear = dt->id_start++;
}

void dict_release(dict_t *dt)
{
    for (uint32_t i = 0; i < dt->size && dt->used > 0; ++i) {
        dict_entry *entry = dt->table[i];
        dict_entry *next_entry = NULL;
        while (entry) {
            next_entry = entry->next;
            DICT_FREE_HASH_KEY(dt, entry);
            DICT_FREE_HASH_VAL(dt, entry);
            free(entry);
            entry = next_entry;
        }
    }
    free(dt->table);
    free(dt);
}

dict_iterator *dict_get_iterator(dict_t *dt)
{
    dict_iterator *iter = malloc(sizeof(dict_iterator));
    if (iter == NULL)
        return NULL;
    memset(iter, 0, sizeof(dict_iterator));
    iter->dt = dt;
    iter->index = -1;
    iter->entry = NULL;
    iter->next_entry = NULL;

    return iter;
}

dict_entry *dict_next(dict_iterator *iter)
{
    while (1) {
        if (iter->entry == NULL) {
            iter->index++;
            if (iter->index >= iter->dt->size)
                break;
            iter->entry = iter->dt->table[iter->index];
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

void dict_release_iterator(dict_iterator *iter)
{
    free(iter);
}

