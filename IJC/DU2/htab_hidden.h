// htab_hidden.h
// Řešení IJC-DU2, příklad b), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#ifndef __HTAB_HIDDEN_H__
#define __HTAB_HIDDEN_H__

#include "htab.h"

/*
 * Při mém výzkumu o hashovacích tabulkách jsem narazil
 * na https://www.cs.cornell.edu/courses/cs312/2008sp/lectures/lec20.html,
 * kde se problematice zvětšování a zmenšování věnují.
 * Jako ideální hodnoty pro zvětšení a zmenšení autor uvádí 1/4 a 2/3.
 */
#define AVG_LEN_MAX 0.67
#define AVG_LEN_MIN 0.25

typedef struct htab_item
{
    htab_pair_t pair;
    struct htab_item *next;
} htab_item_t;

struct htab
{
    size_t size;
    size_t arr_size;
    htab_item_t **arr_ptr;
};

#endif // __HTAB_H__
