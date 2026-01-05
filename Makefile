# ===============================
# MichOS Makefile
# Author: Michael Effendy
# ===============================

# ----- CONFIG -----
OS_NAME = michos
CC = gcc
ASM = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pic -fno-pie \
         -fno-stack-protector -fno-asynchronous-unwind-tables \
         -fno-unwind-tables -fno-exceptions -nostdlib

LDFLAGS = -m elf_i386

BUILD_DIR = build
ISO_DIR = iso

# ----- OBJECT FILES -----
OBJS = $(BUILD_DIR)/multiboot.o \
       $(BUILD_DIR)/gdt.o \
       $(BUILD_DIR)/vga.o \
       $(BUILD_DIR)/pit.o \
       $(BUILD_DIR)/pic.o \
       $(BUILD_DIR)/idt.o \
       $(BUILD_DIR)/keyboard.o \
       $(BUILD_DIR)/interrupt.o \
       $(BUILD_DIR)/kernel.o


# ===============================
# RULES
# ===============================

# default target
all: $(OS_NAME).iso

# build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# assemble multiboot header
$(BUILD_DIR)/multiboot.o: multiboot.asm | $(BUILD_DIR)
	$(ASM) -f elf32 $< -o $@

# compile VGA driver
$(BUILD_DIR)/vga.o: vga.c vga.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# compile PIT driver
$(BUILD_DIR)/pit.o: pit.c pit.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# compile kernel
$(BUILD_DIR)/kernel.o: kernel.c vga.h pit.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# assemble interrupt handler
$(BUILD_DIR)/interrupt.o: interrupt.asm | $(BUILD_DIR)
	nasm -f elf32 $< -o $@

# compile PIC driver
$(BUILD_DIR)/pic.o: pic.c pic.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# compile IDT driver
$(BUILD_DIR)/idt.o: idt.c idt.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# assemble GDT driver
$(BUILD_DIR)/gdt.o: gdt.asm | $(BUILD_DIR)
	nasm -f elf32 $< -o $@

# compile keyboard driver
$(BUILD_DIR)/keyboard.o: keyboard.c keyboard.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# link kernel
$(BUILD_DIR)/kernel.bin: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -T linker.ld $(OBJS) -o $@

# create bootable ISO
$(OS_NAME).iso: $(BUILD_DIR)/kernel.bin grub.cfg
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(BUILD_DIR)/kernel.bin $(ISO_DIR)/boot/kernel.bin
	cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(OS_NAME).iso $(ISO_DIR) >/dev/null 2>&1

# run in QEMU
run: $(OS_NAME).iso
	qemu-system-i386 -cdrom $(OS_NAME).iso

# clean build artifacts
clean:
	rm -rf $(BUILD_DIR) *.iso
