// htab_lookup_add.c
// Řešení IJC-DU2, příklad a), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include "htab_hidden.h"
#include <stdlib.h>

htab_pair_t *htab_lookup_add(htab_t *t, htab_key_t key)
{
    if (t == NULL)
        return NULL;

    htab_pair_t *found = htab_find(t, key);

    if (found != NULL)
        return found;

    htab_item_t *newitem = malloc(sizeof(htab_item_t));
    if (newitem == NULL)
    {
        return NULL;
    }
    size_t len = strlen(key);
    newitem->pair.key = calloc(len + 1, 1);
    if (newitem->pair.key == NULL)
    {
        free(newitem);
        return NULL;
    }

    memcpy((char *)newitem->pair.key, key, len);
    newitem->pair.value = 0;
    newitem->next = NULL;
    t->size++;

    if (((float)t->size / t->arr_size) > AVG_LEN_MAX)
        htab_resize(t, 2 * t->arr_size);

    size_t index = htab_hash_function(key) % t->arr_size;
    htab_item_t *tmp = t->arr_ptr[index];

    if (tmp == NULL)
        t->arr_ptr[index] = newitem;
    else
    {
        while (tmp->next != NULL)
            tmp = tmp->next;

        tmp->next = newitem;
    }

    return &(newitem->pair);
}
