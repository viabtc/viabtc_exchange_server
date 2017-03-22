/*
 * Description: 
 *     History: yang@haipo.me, 2017/03/21, create
 */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "ut_sds.h"
# include "ut_skiplist.h"

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
    skiplist_type type;
    type.dup = node_dup;
    type.free = node_free;
    type.compare = node_compare;
    skiplist_t *list = skiplist_create(&type);

    for (int i = 0; i < 1000; ++i) {
        int num = random() % 26;
        sds value = sdsempty();
        value = sdscatprintf(value, "%c", 'a' + num);
        if (skiplist_insert(list, value)) {
            printf("insert: %s\n", value);
        }
        sdsfree(value);
    }

    printf("list len: %ld\n", skiplist_len(list));
    printf("level: %d\n", list->level);

    for (int i = list->level - 1; i >= 0; --i) {
        printf("level %d: ", i);
        skiplist_node *node = list->header->forward[i];
        while (node) {
            printf("%s -> ", (char *)node->value);
            node = node->forward[i];
        }
        printf("\n");
    }

    sds value = sdsnew("k");
    skiplist_delete(list, skiplist_find(list, value));
    sdsfree(value);

    printf("list len: %ld\n", skiplist_len(list));
    printf("level: %d\n", list->level);

    skiplist_iter *iter = skiplist_get_iterator(list);
    skiplist_node *node;
    while ((node = skiplist_next(iter)) != NULL) {
        printf("%s\n", (char *)node->value);
        skiplist_delete(list, node);
    }
    printf("list len: %ld\n", skiplist_len(list));

    return 0;
}

