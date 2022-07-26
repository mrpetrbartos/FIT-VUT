// wordcount.c
// Řešení IJC-DU2, příklad a), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include "htab.h"
#include "io.h"
#include <stdlib.h>

/*
 * Publikace Introduction to Algorithms uvádí, že pro
 * získání optimálních výsledků je ideální jako velikost
 * zvolit prvočíslo, které není blízko nté mocniny čísla 2.
 * https://blog.reedsy.com/how-many-words-in-a-novel/
 * stránka výše uvádí jako průměrný počet slov 40 000,
 * hledal jsem tedy prvočísla kolem této velikosti
 * a 37573 se mi zalíbilo, protože je palindrom.
 */

#define INITIAL_SIZE 37573

#define MAX_LEN 127

void print_pair(htab_pair_t *data)
{
    printf("%s\t%d\n", data->key, data->value);
}

int main()
{
    htab_t *t = htab_init(INITIAL_SIZE);
    if (t == NULL)
        return 1;

    char str[MAX_LEN + 1] = {0};

    htab_pair_t *word;
    while (read_word(str, MAX_LEN, stdin) != EOF)
    {
        word = htab_lookup_add(t, str);
        if (word == NULL)
        {
            htab_free(t);
            return 1;
        }
        word->value++;
    }

    htab_for_each(t, print_pair);

    htab_free(t);
    return 0;
}
