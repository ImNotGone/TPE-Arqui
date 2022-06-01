
all: bootloader kernel userland image
gdb: all kernel_elf userland_elf

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all

kernel_elf:
	cd Kernel; make gdb

userland:
	cd Userland; make all

userland_elf:
	cd Userland; make gdb

image: kernel bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: bootloader image collections kernel userland all clean
