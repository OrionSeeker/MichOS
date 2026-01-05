# MichOS

MichOS is a simple educational operating system written from scratch in C.
This project is created for learning low-level system programming, kernel
development, and x86 architecture.

## Author
**Michael Effendy**

## Features
- Multiboot compliant (GRUB)
- 32-bit x86 kernel
- VGA text mode output
- Keyboard input (polling)
- Simple shell

## Build Requirements
- GCC (32-bit support)
- NASM
- GRUB
- QEMU

## Build & Run
```bash
nasm -f elf32 multiboot.asm -o multiboot.o

gcc -m32 -ffreestanding -fno-pic -fno-pie \
-fno-stack-protector -fno-asynchronous-unwind-tables \
-fno-unwind-tables -fno-exceptions -nostdlib \
-c vga.c -o vga.o

gcc -m32 -ffreestanding -fno-pic -fno-pie \
-fno-stack-protector -fno-asynchronous-unwind-tables \
-fno-unwind-tables -fno-exceptions -nostdlib \
-c kernel.c -o kernel.o

ld -m elf_i386 -T linker.ld \
multiboot.o vga.o kernel.o -o kernel.bin

mkdir -p iso/boot/grub
cp kernel.bin iso/boot/kernel.bin
cp grub.cfg iso/boot/grub/grub.cfg
grub-mkrescue -o michos.iso iso

qemu-system-i386 -cdrom michos.iso
