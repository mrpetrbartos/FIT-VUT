// io.c
// Řešení IJC-DU2, příklad b), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include "io.h"
#include <ctype.h>
#include <string.h>

int read_word(char *s, int max, FILE *f)
{
    if (f == NULL)
        return EOF;

    int c;
    int i = 0;
    static int warning = 0;

    while ((c = fgetc(f)) != EOF && isspace(c))
        ;

    if (c == EOF)
        return EOF;

    s[i] = c;
    i++;

    while ((c = fgetc(f)) != EOF && !isspace(c))
    {
        if (i >= max - 1)
        {
            if (warning == 0)
            {
                fprintf(stderr, "CHYBA: byla presazena maximalni delka.\n");
                warning = 1;
            }
            continue;
        }
        s[i] = c;
        i++;
    }

    s[i] = '\0';

    return strlen(s);
}
