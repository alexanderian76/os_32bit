ASM=nasm

BUILD_DIR=build
KERNEL_DIR=kernel
TOOLS_DIR=tools

all: second run2


first:
	nasm -f bin boot.asm -o boot.bin
second:
	dd if=/dev/zero of=bin/main_floppy.img bs=512 count=2880
	mkfs.fat -F 12 -n "NBOS" bin/main_floppy.img 

	nasm -f bin boot.asm -o bin/boot.bin
	nasm kernel.asm -f elf -o ./bin/kernel_entry.o
	gcc -ffreestanding -m32 -g -c kernel.c -o bin/kernel.o
	nasm zeroes.asm -f bin -o bin/zeroes.bin
	ld  -m elf_i386 -o bin/full_kernel.bin -Ttext 0x1000 bin/kernel.o bin/kernel_entry.o   --oformat binary
	
	cat bin/boot.bin bin/full_kernel.bin bin/zeroes.bin  > bin/OS.bin
	dd if=bin/OS.bin of=bin/main_floppy.img conv=notrunc 
run1:
	qemu-system-i386 -drive format=raw,file="bin/OS.bin",index=0,if=floppy,  -m 128M
run2:
	qemu-system-i386 -fda bin/main_floppy.img
run:
	qemu-system-i386 boot.bin 