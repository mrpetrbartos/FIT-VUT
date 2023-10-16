/**
 * @file ian-proj1.c
 * @author Petr Bartoš (xbarto0g)
 * @brief Simple program for parsing ELF files
 * @date 2023-03-12
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <gelf.h>
#include <elf.h>

#define CASE(P_TYPE)                \
    case PT_##P_TYPE:               \
        printf("%-12s\t", #P_TYPE); \
        break

int checkSections(GElf_Shdr *secHeader, GElf_Phdr *header)
{
    if (secHeader->sh_addr == 0)
        return 0;

    if (header->p_vaddr > secHeader->sh_addr)
        return 0;

    if (header->p_vaddr + header->p_memsz < secHeader->sh_addr + secHeader->sh_size)
        return 0;

    return 1;
}

int getSectionInfo(Elf *elfFile, int ord, GElf_Phdr *header, size_t strTableIndex)
{
    printf("%02d\t", ord);

    switch (header->p_type)
    {
        CASE(NULL);
        CASE(LOAD);
        CASE(DYNAMIC);
        CASE(INTERP);
        CASE(NOTE);
        CASE(SHLIB);
        CASE(PHDR);
        CASE(TLS);
        CASE(NUM);
        CASE(LOOS);
        CASE(GNU_EH_FRAME);
        CASE(GNU_STACK);
        CASE(GNU_RELRO);
        CASE(GNU_PROPERTY);
        CASE(LOSUNW);
        CASE(SUNWSTACK);
        CASE(HISUNW);
        CASE(LOPROC);
        CASE(HIPROC);
    default:
        printf("%-12s", "undefined\t");
        break;
    }

    switch (header->p_flags)
    {
    case 0:
        printf("---\t");
        break;
    case 1:
        printf("--X\t");
        break;
    case 2:
        printf("-W-\t");
        break;
    case 3:
        printf("-WX\t");
        break;
    case 4:
        printf("R--\t");
        break;
    case 5:
        printf("R-X\t");
        break;
    case 6:
        printf("RW-\t");
        break;
    case 7:
        printf("RWX\t");
        break;
    default:
        printf("???\t");
    }

    Elf_Scn *secDesc = elf_nextscn(elfFile, NULL);
    GElf_Shdr secHeader;
    while (secDesc != NULL)
    {
        if (gelf_getshdr(secDesc, &secHeader) == NULL)
        {
            fprintf(stderr, "Couldn't fetch a pointer to a section header");
            return 1;
        }

        if (checkSections(&secHeader, header))
        {
            char *section = elf_strptr(elfFile, strTableIndex, secHeader.sh_name);
            printf("%s ", section);
        }

        secDesc = elf_nextscn(elfFile, secDesc);
    }

    printf("\n");
    return 0;
}

int parseElf(Elf *elfFile)
{
    size_t programHeadersCnt;
    if (elf_getphdrnum(elfFile, &programHeadersCnt) != 0)
    {
        fprintf(stderr, "Couldn't fetch the number of program headers\n");
        return 1;
    }

    size_t strTableIndex;
    if (elf_getshdrstrndx(elfFile, &strTableIndex) != 0)
    {
        fprintf(stderr, "Couldn't fetch the section index of the string table\n");
        return 1;
    }

    printf("Segment\tType\t\tPerm\tSections\n");

    for (int i = 0; i < programHeadersCnt; i++)
    {
        GElf_Phdr programHeader;
        if (gelf_getphdr(elfFile, i, &programHeader) == NULL)
        {
            fprintf(stderr, "Error was encountered when trying to translate ELF program header information\n");
            return 1;
        }
        if (getSectionInfo(elfFile, i, &programHeader, strTableIndex) != 0)
            return 1;
    }

    return 0;
}

Elf *getElf(int fd)
{
    Elf *elfDesc = elf_begin(fd, ELF_C_READ, NULL);
    if (elfDesc == NULL)
    {
        fprintf(stderr, "Couldn't read contents of the file\n");
        return NULL;
    }

    Elf_Kind ek = elf_kind(elfDesc);
    if (ek != ELF_K_ELF)
    {
        fprintf(stderr, "Provided file isn't an ELF\n");
        return NULL;
    }

    return elfDesc;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Invalid number of arguments passed\n");
        printf("Usage: ./ian-proj1 FILE\n");
        return 1;
    }

    if (elf_version(EV_CURRENT) == EV_NONE)
    {
        fprintf(stderr, "Invalid ELF version\n");
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        fprintf(stderr, "Couldn't open provided file\n");
        return 1;
    }

    Elf *elfFile = getElf(fd);
    if (elfFile == NULL)
    {
        elf_end(elfFile);
        close(fd);
        return 1;
    }

    int parseResult = parseElf(elfFile);
    elf_end(elfFile);
    close(fd);
    return parseResult;
}
