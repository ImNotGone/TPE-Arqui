#ifndef _STDIO_H
#define _STDIO_H
#include <stdarg.h>
#include <stdint.h>
#include <syscalls.h>

#define BUFF_SIZE 8096
// File Descriptors
#define STDIN   0
#define STDOUT  1
#define STDERR  2

// Prints the specified format into the fd
int64_t fprintf(uint64_t fd, const char * fmt, ...);

// Prints the specified format into STDOUT
int64_t printf(const char * fmt, ...);

// Scans the specified format from STDIN
int64_t scanf(const char * fmt, ...);

// Prints the string and a newline into the fd
int64_t fputs(uint64_t fd, const char * str);

// Prints the string and a newline into STDOUT
int64_t puts(const char * str);

// Prints the char into STDOUT
int64_t putchar(int64_t c);

// Gets a char from STDIN
int64_t getchar();

#endif//_STDIO_H