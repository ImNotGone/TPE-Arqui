#include <dumps.h>

void memDump(uint64_t address) {
    int8_t memData[MEM_DUMP_COUNT];
    int64_t accessed = sysmemdump(address, memData);
    if (accessed < 0) {
        fputs(STDERR, MEM_DUMP_ERR);
        return;
    }

    printf("Printing %d bytes starting at address %x:\n", accessed, address);
    for (int i = 0; i < accessed; i++)
        printf("%x: %x\n", address + i, 0xFF & memData[i]);
    if(accessed < MEM_DUMP_COUNT) {
        fprintf(STDERR, MAX_MEM_NOTICE);
    }
}

void regDump() {
    TRegs regs;
    int8_t regSaved = sysregdump(&regs); //sysregdump retorna 0 si no habia un snapshot
    if(!regSaved) {
        puts(REG_DUMP_ERR);
        return;
    }

    puts("Register snapshot:");
    printf("RAX: %x\n", regs.rax);
    printf("RBX: %x\n", regs.rbx);
    printf("RCX: %x\n", regs.rcx);
    printf("RDX: %x\n", regs.rdx);
    printf("RSI: %x\n", regs.rsi);
    printf("RDI: %x\n", regs.rdi);
    printf("RBP: %x\n", regs.rbp);
    printf("RSP: %x\n", regs.rsp);
    printf("R8:  %x\n", regs.r8 );
    printf("R9:  %x\n", regs.r9 );
    printf("R10: %x\n", regs.r10);
    printf("R11: %x\n", regs.r11);
    printf("R12: %x\n", regs.r12);
    printf("R13: %x\n", regs.r13);
    printf("R14: %x\n", regs.r14);
    printf("R15: %x\n", regs.r15);
    printf("RIP: %x\n", regs.rip);
}