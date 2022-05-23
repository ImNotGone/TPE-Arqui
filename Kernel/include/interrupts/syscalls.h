#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

typedef struct time {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} Ttime;

int64_t syscallDispatcher(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8);

#endif//SYSCALLS_H