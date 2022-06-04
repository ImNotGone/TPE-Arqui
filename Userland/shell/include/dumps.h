#ifndef _DUMPS_H
#define _DUMPS_H
#include <_stdio.h>

#define MEM_DUMP_COUNT 32

void memDump(int64_t address);
void regDump();

#endif