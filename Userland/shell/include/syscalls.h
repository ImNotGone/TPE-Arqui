#ifndef SYSCALLS_H_
#define SYSCALLS_H_
#include <stdint.h>

extern int64_t sysread(uint64_t fd, char * buff, int64_t bytes);
extern int64_t syswrite(uint64_t fd, const char * buff, int64_t bytes);
extern void systime(uint32_t * dateData);
extern void sysmemdump(uint64_t address, int8_t *memData);

#endif//SYSCALLS_H_