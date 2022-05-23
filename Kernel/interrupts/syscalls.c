#include <interrupts/syscalls.h>
#include <drivers/naiveConsole.h>
#include <drivers/keyboard.h>
#include <drivers/RTC.h>
#include <stdint.h>

#define STDIN   0
#define STDOUT  1
#define STDERR  2

#define MEM_DUMP_SIZE 32

// rax -> rdi, 
// rdi -> rsi, 
// rsi -> rdx, 
// rdx -> rcx, 
// r10 -> r8,
// r8 ->  r9
typedef int64_t (*TSyscallHandler) (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8);

int64_t sysread(uint64_t fd, char * buffer, int64_t bytes);
int64_t syswrite(uint64_t fd, const char * buffer, int64_t bytes);
void systime(uint32_t *dateData);
void sysmemdump(uint64_t direction, int8_t *memData);


TSyscallHandler syscallHandlers[] = {
    //0x00
    (TSyscallHandler) sysread,
    //0x01
    (TSyscallHandler) syswrite,
    //0x02
    (TSyscallHandler) systime,
    //0x03
    (TSyscallHandler) sysmemdump
};

static uint64_t syscallHandlersDim = sizeof(syscallHandlers) / sizeof(syscallHandlers[0]);

int64_t syscallDispatcher(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8) {
    //ncPrint("rax: ");
    //ncPrintHex(rax);
    //ncNewline();
    //ncPrint("rdi: ");
    //ncPrintHex(rdi);
    //ncNewline();
    //ncPrint("rsi: ");
    //ncPrintHex(rsi);
    //ncNewline();
    //ncPrint("rdx: ");
    //ncPrintHex(rdx);
    //ncNewline();
    //ncPrint("r10: ");
    //ncPrintHex(r10);
    //ncNewline();
    //ncPrint("r8 : ");
    //ncPrintHex(r8);
    //ncNewline();
    if(rax >= syscallHandlersDim)
        return -1;
    
    // function call
    return syscallHandlers[rax](rdi, rsi, rdx, r10, r8);
}

int64_t syswrite(uint64_t fd, const char * buffer, int64_t bytes) {
    if(fd != STDOUT && fd != STDERR) return -1;
    int64_t bytesWritten;
    uint8_t color = (fd == STDOUT)? WHITE:RED;
    for(bytesWritten = 0; bytesWritten < bytes; bytesWritten++) {
        if(buffer[bytesWritten] == '\n') {
            ncNewline();
        } else if(buffer[bytesWritten] == '\b') {
            ncBackSpace();
        } else
            ncPrintCharAtt(buffer[bytesWritten], color);
    }
    return bytesWritten;
}

int64_t sysread(uint64_t fd, char * buffer, int64_t bytes) {
    //ncPrint("fd: ");
    //ncPrintHex(fd);
    //ncNewline();
    //ncPrint("buffer: ");
    //ncPrintHex(buffer);
    //ncNewline();
    //ncPrint("bytes: ");
    //ncPrintHex(bytes);
    if(fd != STDIN) return -1;
    int64_t i = 0;
    char c;
    for(;i < bytes && (c = getchar()) != 0; i++) {
        buffer[i] = c;
    }
    return i;
}

// Escribe la info recibida del rtc en dateData[6]
// dateData[0] = year, dateData[1] = month, dateData[2] = day
// dateData[3] = hours, dateData[4] = minutes, dateData[5] = seconds
// todo otra opcion es que devuelva una estructura con los datos
// pero tanto kernel como userland deberian tener la definicion de esa estructura?
void systime(uint32_t *dateData) {
    dateData[0] = getRTCYear();
    dateData[1] = getRTCMonth();
    dateData[2] = getRTCDayOfMonth();
    dateData[3] = getRTCHours();
    dateData[4] = getRTCMinutes();
    dateData[5] = getRTCSeconds();
}

// Escribe los MEM_DUMP_SIZE bytes desde la direccion de memoria indicada en memData
void sysmemdump(uint64_t direction, int8_t *memData) {
    int8_t *memDir = (int8_t *) direction;
    for (int i = 0; i < MEM_DUMP_SIZE - 1; i++)
        memData[i] = *(memDir + i);
}