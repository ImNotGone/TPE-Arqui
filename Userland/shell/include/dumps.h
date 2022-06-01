#ifndef _DUMP_H
#define _DUMP_H
#include <_stdio.h>

#define MEM_DUMP_COUNT 32

void memDump(uint64_t address);
void regDump();

#endif