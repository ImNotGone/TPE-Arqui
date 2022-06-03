#ifndef _STRING_H
#define _STRING_H
#define LONG_MIN -1L

#include <stdint.h>

uint64_t strlen(const char * str);
int64_t strncmp(const char * str1, const char * str2, uint64_t n);
int64_t strcmp(const char * str1, const char * str2);
int64_t strtol(const char *str, const char **endptr, int base);
int64_t strDivide(const char * str, char * buff1, char * buff2, char divider);
void trim(char *str);

#endif//_STRING_H