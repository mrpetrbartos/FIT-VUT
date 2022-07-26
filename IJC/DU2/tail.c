// tail.c
// Řešení IJC-DU2, příklad a), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define STRING_LIMIT 4095

void free_lines(char **lines, int limit)
{
    for (int i = 0; i < limit; i++)
    {
        free(lines[i]);
    }
    free(lines);
}

int main(int argc, char const *argv[])
{
    FILE *fileptr = stdin;
    unsigned long lines_to_print = 10;
    int warning = 0;

    switch (argc)
    {
    case (1):
        break;
    case (2):
        // expected: ./tail file
        fileptr = fopen(argv[1], "r");
        if (!fileptr)
        {
            fprintf(stderr, "CHYBA: nelze otevrit uvedeny soubor.\n");
            return EXIT_FAILURE;
        }
        break;

    case (3):
    case (4):
        // expected: ./tail -n 111 [FILE]
        if (strcmp("-n", argv[1]) != 0)
        {
            fprintf(stderr, "CHYBA: uvedeny spatny parametr.\n");
            return EXIT_FAILURE;
        }

        if (!(argv[2][0] >= '0' && argv[2][0] <= '9'))
        {
            fprintf(stderr, "CHYBA: pocet radku neni nezaporne cislo.\n");
            return EXIT_FAILURE;
        }
        char *ptr = "";
        unsigned long length = strtoul(argv[2], &ptr, 10);
        if (!((ptr != NULL) && (ptr[0] == '\0')))
        {
            fprintf(stderr, "CHYBA: pocet radku neni nezaporne cislo.\n");
            return EXIT_FAILURE;
        }
        if (length > ULONG_MAX)
            length = ULLONG_MAX;

        lines_to_print = length;

        if (argc != 4)
            break;

        fileptr = fopen(argv[3], "r");
        if (!fileptr)
        {
            fprintf(stderr, "CHYBA: nelze otevrit uvedeny soubor.\n");
            return EXIT_FAILURE;
        }

        break;

    default:
        fprintf(stderr, "CHYBA: bylo pouzito prilis mnoho parametru.\n");
        return EXIT_FAILURE;
    }

    if (lines_to_print == 0)
    {
        if (fileptr != stdin)
            fclose(fileptr);
        exit(0);
    }

    char **lines = malloc(lines_to_print * sizeof(char *));
    if (lines == NULL)
    {
        fprintf(stderr, "CHYBA: pri alokaci pameti doslo k chybe.\n");
        return EXIT_FAILURE;
    }

    for (unsigned long i = 0; i < lines_to_print; i++)
    {
        lines[i] = malloc((STRING_LIMIT + 2) * sizeof(char));
        if (lines[i] == NULL)
        {
            fprintf(stderr, "CHYBA: pri alokaci pameti doslo k chybe.\n");
            free_lines(lines, i);
            if (fileptr != stdin)
                fclose(fileptr);
            return EXIT_FAILURE;
        }
    }

    unsigned long lines_written = 0;
    unsigned long arr_pos = 0;
    while (fgets(lines[arr_pos], STRING_LIMIT + 2, fileptr))
    {
        char *line = lines[arr_pos];
        lines_written++;
        arr_pos = lines_written % lines_to_print;

        if (line[strlen(line) - 1] != '\n')
        {
            line[strlen(line) - 1] = '\n';
            if (warning == 0)
            {
                fprintf(stderr, "CHYBA: radek %ld je delsi nez limit.\n", lines_written);
                warning = 1;
            }
            char ch;
            while ((ch = fgetc(fileptr)) != '\n' && ch != EOF)
                ;
        }
    }

    long print_lines_num = lines_to_print;
    if (lines_to_print > lines_written)
        print_lines_num = lines_written;

    for (unsigned long i = arr_pos; i < arr_pos + print_lines_num; i++)
    {
        long indx = i % print_lines_num;
        printf("%s", lines[indx]);
    }

    free_lines(lines, lines_to_print);

    if (fileptr != stdin)
        fclose(fileptr);
}
