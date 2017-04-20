/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/21, create
 */

# include <stdlib.h>
# include <string.h>

# include "ut_list.h"

list_t *list_create(list_type *type)
{
    list_t *list = malloc(sizeof(list_t));
    if (list == NULL) {
        return NULL;
    }
    memset(list, 0, sizeof(list_t));
    if (type) {
        memcpy(&list->type, type, sizeof(list_type));
    }
    return list;
}

static list_node *list_create_node(list_t *list, void *value)
{
    list_node *node = malloc(sizeof(list_node));
    if (node == NULL) {
        return NULL;
    }
    if (list->type.dup) {
        node->value = list->type.dup(value);
    } else {
        node->value = value;
    }
    if (node->value == NULL) {
        free(node);
        return NULL;
    }
    return node;
}

list_t *list_add_node_head(list_t *list, void *value)
{
    list_node *node = list_create_node(list, value);
    if (node == NULL) {
        return NULL;
    }
    if (list->len == 0) {
        node->next = node->prev = NULL;
        list->head = list->tail = node;
    } else {
        node->next = list->head;
        node->prev = NULL;
        list->head->prev = node;
        list->head = node;
    }
    list->len += 1;
    return list;
}

list_t *list_add_node_tail(list_t *list, void *value)
{
    list_node *node = list_create_node(list, value);
    if (node == NULL) {
        return NULL;
    }
    if (list->len == 0) {
        node->next = node->prev = NULL;
        list->head = list->tail = node;
    } else {
        node->next = NULL;
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }
    list->len += 1;

    return list;
}

list_t *list_insert_node(list_t *list, list_node *pos, void *value, int before)
{
    list_node *node = list_create_node(list, value);
    if (node == NULL) {
        return NULL;
    }
    if (before) {
        node->next = pos;
        node->prev = pos->prev;
        if (list->head == pos) {
            list->head = node;
        }
    } else {
        node->next = pos->next;
        node->prev = pos;
        if (list->tail == pos) {
            list->tail = node;
        }
    }
    if (node->prev != NULL) {
        node->prev->next = node;
    }
    if (node->next != NULL) {
        node->next->prev = node;
    }
    list->len += 1;
    return list;
}

void list_rotate(list_t *list)
{
    if (list->len <= 1)
        return;
    list_node *node = list->tail;
    list->tail = list->tail->prev;
    list->tail->next = NULL;
    node->next = list->head;
    node->prev = NULL;
    list->head->prev = node;
    list->head = node;
}

void list_del(list_t *list, list_node *node)
{
    if (node->next) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }
    if (list->type.free) {
        list->type.free(node->value);
    }
    free(node);
    list->len -= 1;
}

void list_clear(list_t *list)
{
    unsigned long len = list->len;
    list_node *curr, *next;
    curr = list->head;
    while (len--) {
        next = curr->next;
        if (list->type.free) {
            list->type.free(curr->value);
        }
        free(curr);
        curr = next;
    }
    list->len = 0;
    list->head = NULL;
    list->tail = NULL;
}

void list_release(list_t *list)
{
    unsigned long len = list->len;
    list_node *curr, *next;
    curr = list->head;
    while (len--) {
        next = curr->next;
        if (list->type.free) {
            list->type.free(curr->value);
        }
        free(curr);
        curr = next;
    }
    free(list);
}

list_iter *list_get_iterator(list_t *list, int direction)
{
    list_iter *iter = malloc(sizeof(list_iter));
    if (iter == NULL) {
        return NULL;
    }
    if (direction == LIST_START_HEAD) {
        iter->direction = LIST_START_HEAD;
        iter->next = list->head;
    } else {
        iter->direction = LIST_START_TAIL;
        iter->next = list->tail;
    }
    return iter;
}

list_node *list_next(list_iter *iter)
{
    list_node *curr = iter->next;
    if (curr) {
        if (iter->direction == LIST_START_HEAD) {
            iter->next = curr->next;
        } else {
            iter->next = curr->prev;
        }
    }
    return curr;
}

void list_rewind_head(list_t *list, list_iter *iter)
{
    iter->next = list->head;
    iter->direction = LIST_START_HEAD;
}

void list_rewind_tail(list_t *list, list_iter *iter)
{
    iter->next = list->tail;
    iter->direction = LIST_START_TAIL;
}

void list_release_iterator(list_iter *iter)
{
    free(iter);
}

list_t *list_dup(list_t *orig)
{
    list_iter *iter = list_get_iterator(orig, LIST_START_HEAD);
    if (iter == NULL) {
        return NULL;
    }
    list_t *copy = list_create(&orig->type);
    if (copy == NULL) {
        list_release_iterator(iter);
        return NULL;
    }

    list_node *node;
    while ((node = list_next(iter)) != NULL) {
        void *value = node->value;
        if (copy->type.dup) {
            value = copy->type.dup(value);
            if (value == NULL) {
                list_release_iterator(iter);
                list_release(copy);
                return NULL;
            }
        }
        if (list_add_node_tail(copy, value) == NULL) {
            list_release_iterator(iter);
            list_release(copy);
            return NULL;
        }
    }
    list_release_iterator(iter);
    return copy;
}

list_node *list_find(list_t *list, void *value)
{
    if (list->type.compare == NULL)
        return NULL;
    list_iter *iter = list_get_iterator(list, LIST_START_HEAD);
    if (iter == NULL) {
        return NULL;
    }
    list_node *node;
    while ((node = list_next(iter)) != NULL) {
        if (list->type.compare(node->value, value) == 0) {
            list_release_iterator(iter);
            return node;
        }
    }
    list_release_iterator(iter);
    return NULL;
}

list_node *list_index(list_t *list, long index)
{
    list_node *node;
    if (index < 0) {
        node = list->tail;
        index = (-index) - 1;
        while (index-- && node) {
            node = node->prev;
        }
    } else {
        node = list->head;
        while (index-- && node) {
            node = node->next;
        }
    }
    return node;
}

