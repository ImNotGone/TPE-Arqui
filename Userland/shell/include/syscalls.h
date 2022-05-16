#ifndef SYSCALLS_H_
#define SYSCALLS_H_

extern sysread(uint64_t fd, char * buff, uint64_t bytes);
extern syswrite(uint64_t fd, char * buff, uint64_t bytes);

#endif//SYSCALLS_H_