// htab_find.c
// Řešení IJC-DU2, příklad a), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include "htab_hidden.h"

htab_pair_t *htab_find(htab_t *t, htab_key_t key)
{
    if (t == NULL)
        return NULL;

    size_t hash = htab_hash_function(key) % t->arr_size;
    htab_item_t *tmp = t->arr_ptr[hash];

    while (tmp != NULL)
    {
        if (!strcmp(key, tmp->pair.key))
        {
            return &(tmp->pair);
        }
        tmp = tmp->next;
    }
    return NULL;
}
