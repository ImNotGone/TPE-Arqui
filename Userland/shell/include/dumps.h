#ifndef _DUMPS_H
#define _DUMPS_H
#include <_stdio.h>

#define MEM_DUMP_COUNT 32
#define MEM_DUMP_ERR "Cant access any memory from the address received"
#define MAX_MEM_NOTICE "Memory past this point could not be accessed\n"
#define REG_DUMP_ERR "No register snapshot found, press \'ctrl + s\' to take a snapshot"

void memDump(uint64_t address);
void regDump();

#endif