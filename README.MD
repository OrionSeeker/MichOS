# MichOS

MichOS is a simple educational operating system written from scratch in C and Assembly.
This project is designed as a hands-on learning platform for low-level system programming,
kernel development, and the x86 architecture.

MichOS is **not intended for production use**, but focuses on understanding how a real
operating system works internally.

## Author
**Michael Effendy**  
GitHub: OrionSeeker

---

## Current Version
**v0.3**

---

## Features

### Boot & Architecture
- Multiboot compliant (GRUB)
- 32-bit x86 protected mode kernel
- Custom Global Descriptor Table (GDT)
- Interrupt Descriptor Table (IDT)
- Programmable Interrupt Controller (PIC) remapping
- Programmable Interval Timer (PIT)

### Kernel
- Interrupt-safe kernel design
- Custom kernel stack
- Hardware timer interrupt (IRQ0)
- System uptime tracking
- Kernel sleep function using PIT

### Input & Shell
- Interrupt-driven keyboard input (IRQ1)
- Ring buffer for keyboard events
- Modular shell architecture
- Command history navigation (↑ ↓)
- TAB auto-completion for shell commands
- Built-in commands:
  - help
  - clear
  - uptime
  - sleep N

### Output
- VGA text mode output
- Basic text rendering and screen control

---

## Build Requirements
- GCC (with 32-bit multilib support)
- NASM
- GRUB tools (grub-mkrescue)
- QEMU

---

## Build & Run

Build and run MichOS using the provided Makefile:

```bash
make clean
make run
```

This will:
1. Assemble and compile the kernel
2. Link the kernel binary
3. Generate a bootable ISO image
4. Run MichOS in QEMU

---