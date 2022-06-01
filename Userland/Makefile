include Makefile.inc

#SAMPLE_DATA=0001-sampleDataModule.bin

all: shell #sampleDataModule
gdb: all shell_gdb

shell:
	cd shell; make all
shell_gdb:
	cd shell; make gdb
#sampleDataModule:
#	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

clean:
	cd shell; make clean
	rm -rf *.bin *.elf


.PHONY: shell all clean
