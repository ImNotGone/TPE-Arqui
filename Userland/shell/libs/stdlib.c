#include <stdlib.h>
#include <syscalls.h>

int64_t printf(const char * fmt, ...) {
    return;
}
int64_t scanf(const char * fmt, ...){
    return;
}
int64_t putchar(int64_t c){
    if(syswrite(1, &c, 1) == -1) {
        return -1;
    }
    return c;
}

int64_t getchar(){
    return;
}
