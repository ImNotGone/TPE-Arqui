#ifndef _STRING_H
#define _STRING_H
#include <stdint.h>

uint64_t strlen(const char * str);
int64_t strncmp(const char * str1, const char * str2, uint64_t n);
int64_t strcmp(const char * str1, const char * str2);

#endif//_STRING_H