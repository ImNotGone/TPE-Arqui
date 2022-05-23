#ifndef SYSCALLS_H_
#define SYSCALLS_H_
#include <stdint.h>

typedef struct time {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} Ttime;

extern int64_t sysread(uint64_t fd, char * buff, int64_t bytes);
extern int64_t syswrite(uint64_t fd, const char * buff, int64_t bytes);
extern int64_t systime(Ttime * ts);

#endif//SYSCALLS_H_