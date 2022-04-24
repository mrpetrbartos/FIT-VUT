// error.h
// Řešení IJC-DU1, příklad b), 20.3.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#ifndef ERROR_H
#define ERROR_H

void warning_msg(const char *fmt, ...);

void error_exit(const char *fmt, ...);

#endif