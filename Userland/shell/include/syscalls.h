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
} TTime;

typedef struct regs {
    int64_t rax;
    int64_t rbx;
    int64_t rcx;
    int64_t rdx;
    int64_t rsi;
    int64_t rdi;
    int64_t rbp;
    int64_t rsp;
    int64_t r8;
    int64_t r9;
    int64_t r10;
    int64_t r11;
    int64_t r12;
    int64_t r13;
    int64_t r14;
    int64_t r15;
    int64_t rip;
} TRegs;

extern int64_t sysread(uint64_t fd, char * buff, int64_t bytes);
extern int64_t syswrite(uint64_t fd, const char * buff, int64_t bytes);
extern void systime(TTime * ts);
extern void sysmemdump(uint64_t address, int8_t *memData);
extern int8_t sysregdump(TRegs *regs);
extern int64_t sysDivWind();
extern int64_t sysSetWind(uint8_t window);
extern int64_t sysOneWind();

#endif//SYSCALLS_H_