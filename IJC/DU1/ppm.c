// ppm.c
// Řešení IJC-DU1, příklad b), 20.3.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include <stdio.h>
#include <stdlib.h>
#include "ppm.h"
#include "error.h"

void ppm_free(struct ppm *p)
{
    free(p);
}

struct ppm * ppm_read(const char * filename)
{
    FILE *image_p;

    if ((image_p = fopen(filename, "r")) == NULL)
    {
        warning_msg("ppm_read: Soubor nejde otevrit\n");
        return NULL;
    }

    char file_type[2];
    unsigned int xsize, ysize;
    unsigned char color;

    if ((fscanf(image_p, "%2c \n%u %u \n%hhu", file_type, &xsize, &ysize, &color) != 4) || file_type[0] != 'P' || file_type[1] != '6')
    {
        warning_msg("ppm_read: Format souboru neodpovida zadani\n");
        fclose(image_p);
        return NULL;
    }

    unsigned long image_size = xsize * ysize * 3;
    if (image_size > MAX_PPM_SIZE)
    {
        warning_msg("ppm_read: Prilis velka velikost souboru\n");
        fclose(image_p);
        return NULL;         
    }

    struct ppm *ppm_ptr = malloc(image_size + sizeof(struct ppm));
    if (ppm_ptr == NULL)
    {
        warning_msg("ppm_read: Chyba pri alokaci pameti pro obrazek\n");
        fclose(image_p);
        return NULL;        
    }

    ppm_ptr->xsize = xsize;
    ppm_ptr->ysize = ysize; 

    if (fread(ppm_ptr->data, sizeof(char), image_size, image_p) != image_size)
    {
        warning_msg("ppm_read: Nastala chyba pri nahravani dat obrazku\n");
        ppm_free(ppm_ptr);
        return NULL;
    }

    fclose(image_p);
    return ppm_ptr;
}