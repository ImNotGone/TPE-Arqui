#ifndef _STDIO_H
#define _STDIO_H
#include <stdarg.h>
#include <stdint.h>
#include <syscalls.h>

#define BUFSIZE 8192
#define EOF     (-1)
#define STDIN   0
#define STDOUT  1
#define STDERR  2

int64_t printf(const char * fmt, ...);
int64_t scanf(const char * fmt, ...);
int64_t putchar(int64_t c);
int64_t getchar();
int64_t puts(const char * str);

#endif//_STDIO_H