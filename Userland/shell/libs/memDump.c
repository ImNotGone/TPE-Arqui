#include <memDump.h>

void memDump(uint64_t address) {
    int8_t memData[MEM_DUMP_COUNT];
    sysmemdump(address, memData);
    printf("Imprimiendo %d bytes desde la posicion %d:\n", MEM_DUMP_COUNT, address);
    for (int i = 0; i < MEM_DUMP_COUNT; i++)
        printf("%x: %x\n", address + i, memData[i]);
}