#include <syscalls.h>
#include <naiveConsole.h>
#include <colors.h>

#define STDOUT 1
#define STDERR 2

// rax -> rdi, 
// rdi -> rsi, 
// rsi -> rdx, 
// rdx -> rcx, 
// r10 -> r8,
// r8 ->  r9
typedef int64_t (*TSyscallHandler) (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8);

int64_t sysread(uint64_t fd, char * buffer, uint64_t bytes);
int64_t syswrite(uint64_t fd, char * buffer, uint64_t bytes);

TSyscallHandler syscallHandlers[] = {
    //0x00
    (TSyscallHandler) sysread,
    //0x01
    (TSyscallHandler) syswrite

};

static uint64_t syscallHandlersDim = sizeof(syscallHandlers)/sizeof(syscallHandlers[0]);

int64_t syscallDispatcher(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8) {
    /*
    ncPrint("rax: ");
    ncPrintHex(rax);
    ncNewline();
    ncPrint("rdi: ");
    ncPrintHex(rdi);
    ncNewline();
    ncPrint("rsi: ");
    ncPrintHex(rsi);
    ncNewline();
    ncPrint("rdx: ");
    ncPrintHex(rdx);
    ncNewline();
    ncPrint("r10: ");
    ncPrintHex(r10);
    ncNewline();
    ncPrint("r8 : ");
    ncPrintHex(r8);
    ncNewline();
    */
    if(rax >= syscallHandlersDim)
        return -1;
    
    // function call
    return syscallHandlers[rax](rdi, rsi, rdx, r10, r8);
}

int64_t syswrite(uint64_t fd, char * buffer, uint64_t bytes) {
    if(fd != 1 && fd != 2) return -1;
    uint64_t bytesWritten;
    uint8_t color = (fd == STDOUT)? WHITE:RED;
    for(bytesWritten = 0; bytesWritten < bytes; bytesWritten++) {
        if(buffer[bytesWritten] == '\n') {
            ncNewline();
        } else
            ncPrintCharAtt(buffer[bytesWritten], color);
    }
    return bytesWritten;
}

int64_t sysread(uint64_t fd, char * buffer, uint64_t bytes) {
    return 0;
}