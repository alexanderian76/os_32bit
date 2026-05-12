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
	gcc -ffreestanding -m32 -fno-pie -g -c kernel.c -o bin/kernel.o

	nasm zeroes.asm -f bin -o bin/zeroes.bin
	ld -m elf_i386 -o bin/full_kernel.bin -T linker.ld  bin/kernel.o bin/kernel_entry.o  --oformat binary
	
	cat bin/boot.bin bin/full_kernel.bin bin/zeroes.bin  > bin/OS.bin
	dd if=bin/OS.bin of=bin/main_floppy.img conv=notrunc 
run1:
	qemu-system-i386 -drive format=raw,file="bin/OS.bin",index=0,if=floppy,  -m 128M
run2:
	qemu-system-i386 -fda bin/main_floppy.img
run:
	qemu-system-i386 boot.bin 
rrrr:
	qemu-system-i386 -machine pc,accel=tcg -device usb-ehci,id=ehci -drive file=bin/main_floppy.img,format=raw,if=none,id=drive0 -device usb-storage,bus=ehci.0,drive=drive0 -m 128
usb:
	dd if=/dev/zero of=bin/main_floppy.img bs=512 count=2880 status=none
	mkfs.fat -F 32 -n "NBOS" bin/main_floppy.img 

	
	nasm kernel.asm -f elf -o ./bin/kernel_entry.o
	nasm kernel_entry.asm -f elf -o ./bin/kernel_entry_true.o
	gcc -ffreestanding -m32 -fno-pie -fstack-protector-strong -g -c kernel.c -o bin/kernel.o
	gcc -ffreestanding -m32 -fno-pie -fstack-protector-strong -g -c vfs.c -o bin/vfs.o
	gcc -ffreestanding -m32 -fno-pie -fstack-protector-strong -g -c string.c -o bin/string.o
	gcc -ffreestanding -m32 -fno-pie -fstack-protector-strong -g -c stdlib.c -o bin/stdlib.o

	nasm zeroes.asm -f bin -o bin/zeroes.bin
	ld -m elf_i386 -o bin/full_kernel.bin -T linker.ld bin/kernel_entry_true.o bin/kernel.o bin/kernel_entry.o bin/vfs.o bin/stdlib.o bin/string.o --oformat binary
	

	nasm -f bin -D KERNEL_SIZE=$(shell stat -c%s bin/full_kernel.bin) boot.asm -o bin/boot.bin

	cat bin/boot.bin bin/full_kernel.bin bin/zeroes.bin  > bin/OS.bin
	dd if=bin/OS.bin of=bin/main_floppy.img conv=notrunc,fsync status=none

	qemu-system-i386 -hda bin/main_floppy.img

test_boot:
	dd if=/dev/zero of=bin/main_floppy.img bs=512 count=2880 status=none
	mkfs.fat -F 32 -n "NBOS" bin/main_floppy.img 
	nasm -f bin test_boot.asm -o bin/boot.bin

clean:
	rm -rf ./bin/*