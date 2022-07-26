// htab_resize.c
// Řešení IJC-DU2, příklad a), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include "htab_hidden.h"
#include <stdlib.h>
#include <stdio.h>

void htab_resize(htab_t *t, size_t newn)
{
    if (t == NULL || newn == 0)
        return;

    struct htab_item **new_arr = malloc(newn * sizeof(struct htab_item *));
    if (new_arr == NULL)
        return;

    for (size_t i = 0; i < newn; i++)
        new_arr[i] = NULL;

    for (size_t i = 0; i < t->arr_size; i++)
    {
        struct htab_item *temp = t->arr_ptr[i];
        while (temp != NULL)
        {
            struct htab_item *next = temp->next;
            temp->next = NULL;
            size_t index = htab_hash_function(temp->pair.key) % newn;
            struct htab_item *curr = new_arr[index];

            if (curr == NULL)
                new_arr[index] = temp;
            else
            {
                while (curr->next != NULL)
                    curr = curr->next;
                curr->next = temp;
            }
            temp = next;
        }
    }

    free(t->arr_ptr);

    t->arr_ptr = new_arr;
    t->arr_size = newn;
}
