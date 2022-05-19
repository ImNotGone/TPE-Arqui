include ../Makefile.inc

MODULE=0000-shell.bin
SOURCES=$(wildcard [^_]*.c */*.c)

all: $(MODULE)

$(MODULE): $(SOURCES)
	$(ASM) $(ASMFLAGS) asm/syscalls.asm
	$(GCC) $(GCCFLAGS) -I./include -T shell.ld $(SOURCES) asm/syscalls.o -o ../$(MODULE)

clean:
	rm -rf */*.o *.o

.PHONY: all clean print
