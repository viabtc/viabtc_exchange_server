/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/20, create
 */

# ifndef _UT_SKIPLIST_H_
# define _UT_SKIPLIST_H_

typedef struct skiplist_node {
    void *value;
    struct skiplist_node *forward[];
} skiplist_node;

typedef struct skiplist_iter {
    skiplist_node *next;
} skiplist_iter;

typedef struct skiplist_type {
    void *(*dup)(void *value);
    void (*free)(void *value);
    int (*compare)(const void *value1, const void *value2);
} skiplist_type;

typedef struct skiplist_t {
    int level;
    skiplist_type type;
    skiplist_node *header;
    unsigned long len;
} skiplist_t;

# define skiplist_len(l)        ((l)->len)
# define skiplist_node_value(n) ((n)->value)

skiplist_t *skiplist_create(skiplist_type *type);
skiplist_t *skiplist_insert(skiplist_t *list, void *value);
skiplist_node *skiplist_find(skiplist_t *list, void *value);
void skiplist_delete(skiplist_t *list, skiplist_node *node);
void skiplist_release(skiplist_t *list);

skiplist_iter *skiplist_get_iterator(skiplist_t *list);
skiplist_node *skiplist_next(skiplist_iter *iter);
void skiplist_release_iterator(skiplist_iter *iter);

# endif

