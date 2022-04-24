// steg-decode.c
// Řešení IJC-DU1, příklad b), 20.3.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include <stdio.h>

#include "error.h"
#include "ppm.h"
#include "eratosthenes.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        error_exit("steg-decode: Program zavolan se spatnym poctem argumentu\n");
    }

    struct ppm *img_struc = ppm_read(argv[1]);
    if (img_struc == NULL)
    {
        error_exit("steg-decode: Nelze ziskat data obrazku\n");
    }

    bitset_index_t sieve_length = img_struc->xsize * img_struc->ysize * 3;

    bitset_alloc(image_sieve, sieve_length);
    Eratosthenes(image_sieve);

    int null_end = 0;
    unsigned char letter = '\0';
    unsigned int pos_offset = 0;
    for (bitset_index_t i = 29; i < sieve_length; i++)
    {
        if (bitset_getbit(image_sieve, i)) continue;
        
        unsigned char lowest_bit = img_struc->data[i+1] & 0x01;
        letter |= lowest_bit << pos_offset;
        pos_offset++;

        if (pos_offset != CHAR_BIT) continue;
        
        printf("%c", letter);
        if (letter == '\0')
        {
            null_end = 1;
            break; 
        }
        pos_offset = 0;
        letter = '\0'; 
    }
    printf("\n");

    bitset_free(image_sieve);
    ppm_free(img_struc);

    if (!null_end)
    {
        error_exit("steg-decode: Na konci zpravy chybi null znak\n");
    }  

    return 0;
}
