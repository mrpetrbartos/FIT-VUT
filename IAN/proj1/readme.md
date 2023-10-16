# Práce s ELF soubory

Vašim úkolem je napsat program, který vypíše základní informace o ELF segmentech, zejména jejich typ, práva a mapování ELF sekcí na ELF segmenty.

Příklad běhu nad standardním "Hello world" může být následující:

```shell
$ ./ian-proj1 hello
Segment Type         Perm Sections
00 PHDR         R--
01 INTERP       R--  .interp
02 LOAD         R--  .interp .note.gnu.build-id .note.ABI-tag .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.dyn .rela.plt
03 LOAD         R-X  .init .plt .text .fini
04 LOAD         R--  .rodata .eh_frame_hdr .eh_frame
05 LOAD         RW-  .init_array .fini_array .dynamic .got .got.plt .data .bss
06 DYNAMIC      RW-  .dynamic
07 NOTE         R--  .note.gnu.build-id .note.ABI-tag
08 GNU_EH_FRAME R--  .eh_frame_hdr
09 GNU_STACK    RW-
10 GNU_RELRO    R--  .init_array .fini_array .dynamic .got
```

## Hodnocení

| Část | Body  | Chyby |
|------|-------|-------|
| 1    | 15/15 | -     |
