#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H
#include <stdint.h>

void exceptionDispatcher(int exception, uint64_t exceptionRegisters[18]);

#endif//_EXCEPTIONS_H