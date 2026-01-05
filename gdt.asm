[bits 32]
global gdt_flush

gdt_start:
    dq 0x0000000000000000    ; null

    ; code segment (0x08)
    dq 0x00CF9A000000FFFF

    ; data segment (0x10)
    dq 0x00CF92000000FFFF

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt_flush:
    lgdt [gdt_descriptor]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.flush
.flush:
    ret
