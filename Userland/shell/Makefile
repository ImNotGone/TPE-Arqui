include ../Makefile.inc

MODULE=0000-shell.bin
MODULE_ELF=0000-shell.elf
SOURCES=$(wildcard [^_]*.c */*.c)

all: $(MODULE)

$(MODULE): $(SOURCES)
	$(ASM) $(ASMFLAGS) asm/syscalls.asm
	$(ASM) $(ASMFLAGS) asm/invalidopcode.asm
	$(ASM) $(ASMFLAGS) asm/zerodiv.asm
	$(GCC) $(GCCFLAGS) -I./include -T shell.ld $(SOURCES) asm/*.o -o ../$(MODULE)
	$(GCC) $(GCCFLAGS) -I./include -T shell.ld -Wl,--oformat=elf64-x86-64 $(SOURCES) asm/*.o -o ../$(MODULE_ELF)

clean:
	rm -rf */*.o *.o

.PHONY: all clean print
