/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/21, create
 */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "ut_sds.h"
# include "ut_list.h"

void *node_dup(void *obj)
{
    return sdsnewlen(obj, sdslen(obj));
}

void node_free(void *obj)
{
    sdsfree(obj);
}

int node_compare(const void *obj, const void *key)
{
    return strcmp(obj, key);
}

int main(int argc, char *argv[])
{
    list_type type;
    type.dup = node_dup;
    type.free = node_free;
    type.compare = node_compare;
    list_t *list = list_create(&type);

    for (int i = 0; i < 10; ++i) {
        sds value = sdsempty();
        value = sdscatprintf(value, "%d", i);
        list_add_node_tail(list, value);
        printf("add %s\n", value);
        sdsfree(value);
    }

    for (int i = 0; i < 10; ++i) {
        sds value = sdsempty();
        value = sdscatprintf(value, "%d", 10 + i);
        list_add_node_head(list, value);
        printf("add %s\n", value);
        sdsfree(value);
    }

    list_node *node = list_index(list, 10);
    sds value = sdsempty();
    value = sdscatprintf(value, "%d", 100);
    list_insert_node(list, node, value, 1);
    printf("insert %s\n", value);

    node = list_find(list, value);
    printf("search: %s\n", (char *)node->value);
    sdsfree(value);

    for (int i = -10; i < 10; ++i) {
        node = list_index(list, i);
        if (node) {
            printf("%d: %s\n", i, (char *)node->value);
        }
    }

    list_t *copy = list_dup(list);
    list_release(list);

    printf("len: %ld\n", list_len(copy));
    list_rotate(copy);
    list_rotate(copy);
    list_rotate(copy);
    list_iter *iter = list_get_iterator(copy, LIST_START_HEAD);
    while ((node = list_next(iter)) != NULL) {
        printf("%s\n", (char *)node->value);
        list_del(copy, node);
    }
    list_release_iterator(iter);
    list_release(copy);
    printf("len: %ld\n", list_len(copy));
    printf("head: %p, tail: %p\n", copy->head, copy->tail);

    return 0;
}

