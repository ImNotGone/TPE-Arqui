include ../Makefile.inc

MODULE=0000-shell.bin
SOURCES=$(wildcard [^_]*.c */*.c)

all: $(MODULE)

$(MODULE): $(SOURCES)
	$(ASM) $(ASMFLAGS) asm/syscalls.asm
	$(ASM) $(ASMFLAGS) asm/invalidopcode.asm
	$(ASM) $(ASMFLAGS) asm/zerodiv.asm
	$(GCC) $(GCCFLAGS) -I./include -T shell.ld $(SOURCES) asm/*.o -o ../$(MODULE)

clean:
	rm -rf */*.o *.o

.PHONY: all clean print
