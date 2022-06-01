include ../Makefile.inc

MODULE=../0000-shell.bin
MODULE_ELF=$(MODULE:.bin=.elf)
MODULE_LD=shell.ld
SOURCES=$(wildcard *.c */*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

all: $(MODULE)
gdb: all $(MODULE_ELF)

$(MODULE): $(OBJECTS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T $(MODULE_LD) -o $(MODULE) $(OBJECTS) $(OBJECTS_ASM)

$(MODULE_ELF):$(OBJECTS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T $(MODULE_LD) --oformat=elf64-x86-64 -o $(MODULE_ELF) $(OBJECTS) $(OBJECTS_ASM)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf */*.o *.o

.PHONY: all clean
