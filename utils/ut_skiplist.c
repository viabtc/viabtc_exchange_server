/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/20, create
 */

# include <stdlib.h>
# include <string.h>

# include "ut_skiplist.h"

# define SKIPLIST_MAX_LEVEL 16
# define SKIPLIST_P         0.25

static skiplist_node *skiplist_create_node(skiplist_t *list, int level, void *value)
{
    size_t size = sizeof(skiplist_node) + level * sizeof(skiplist_node *);
    skiplist_node *node = malloc(size);
    if (node == NULL) {
        return NULL;
    }
    memset(node, 0, size);
    if (value && list->type.dup) {
        node->value = list->type.dup(value);
    } else {
        node->value = value;
    }
    return node;
}

skiplist_t *skiplist_create(skiplist_type *type)
{
    if (type == NULL || type->compare == NULL) {
        return NULL;
    }
    skiplist_t *list = malloc(sizeof(skiplist_t));
    if (list == NULL) {
        return NULL;
    }
    memset(list, 0, sizeof(skiplist_t));
    list->level = 1;
    memcpy(&list->type, type, sizeof(skiplist_type));
    list->header = skiplist_create_node(list, SKIPLIST_MAX_LEVEL, NULL);
    if (list->header == NULL) {
        free(list);
        return NULL;
    }
    return list;
}

static int skiplist_random_level(void)
{
    int level = 1;
    while ((random() & 0xFFFF) < (SKIPLIST_P * 0xFFFF)) {
        level += 1;
    }
    return level < SKIPLIST_MAX_LEVEL ? level : SKIPLIST_MAX_LEVEL;
}

skiplist_t *skiplist_insert(skiplist_t *list, void *value)
{
    skiplist_node *update[SKIPLIST_MAX_LEVEL];
    skiplist_node *node = list->header;

    for (int i = list->level - 1; i >= 0; i--) {
        while (node->forward[i] && list->type.compare(node->forward[i]->value, value) <= 0) {
            node = node->forward[i];
        }
        update[i] = node;
    }
    if (node->value && list->type.compare(node->value, value) == 0) {
        return NULL;
    }

    int level = skiplist_random_level();
    if (level > list->level) {
        for (int i = list->level; i < level; ++i) {
            update[i] = list->header;
        }
        list->level = level;
    }

    node = skiplist_create_node(list, level, value);
    if (node == NULL) {
        return NULL;
    }
    for (int i = 0; i < level; ++i) {
        node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = node;
    }
    list->len += 1;
    return list;
}

skiplist_node *skiplist_find(skiplist_t *list, void *value)
{
    skiplist_node *node = list->header;
    for (int i = list->level - 1; i >= 0; i--) {
        while (node->forward[i] && list->type.compare(node->forward[i]->value, value) <= 0) {
            node = node->forward[i];
        }
    }
    if (node->value && list->type.compare(node->value, value) == 0) {
        return node;
    }
    return NULL;
}

void skiplist_delete(skiplist_t *list, skiplist_node *x)
{
    skiplist_node *update[SKIPLIST_MAX_LEVEL];
    skiplist_node *node = list->header;

    for (int i = list->level - 1; i >= 0; i--) {
        while (node->forward[i] && list->type.compare(node->forward[i]->value, x->value) < 0) {
            node = node->forward[i];
        }
        update[i] = node;
    }

    for (int i = 0; i < list->level; ++i) {
        if (update[i]->forward[i] == x) {
            update[i]->forward[i] = x->forward[i];
        }
    }
    while (list->level > 1 && list->header->forward[list->level - 1] == NULL) {
        list->level -= 1;
    }

    if (list->type.free) {
        list->type.free(x->value);
    }
    free(x);
    list->len -= 1;
}

void skiplist_release(skiplist_t *list)
{
    unsigned long len = list->len;
    skiplist_node *curr = list->header->forward[0];
    skiplist_node *next;
    while (len--) {
        next = curr->forward[0];
        if (list->type.free) {
            list->type.free(curr->value);
        }
        free(curr);
        curr = next;
    }
    free(list->header);
    free(list);
}

skiplist_iter *skiplist_get_iterator(skiplist_t *list)
{
    skiplist_iter *iter = malloc(sizeof(skiplist_iter));
    if (iter == NULL) {
        return NULL;
    }
    iter->next = list->header->forward[0];
    return iter;
}

skiplist_node *skiplist_next(skiplist_iter *iter)
{
    skiplist_node *curr = iter->next;
    if (curr) {
        iter->next = curr->forward[0];
    }
    return curr;
}

void skiplist_release_iterator(skiplist_iter *iter)
{
    free(iter);
}

