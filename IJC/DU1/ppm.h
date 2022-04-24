// ppm.h
// Řešení IJC-DU1, příklad b), 20.3.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#ifndef PPM_H
#define PPM_H

#define MAX_PPM_SIZE (8000*8000*3)

struct ppm {
    unsigned xsize;
    unsigned ysize;
    char data[];
};

struct ppm * ppm_read(const char * filename);

void ppm_free(struct ppm *p);

#endif