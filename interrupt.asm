[bits 32]
global irq0_handler
global irq1_handler

irq0_handler:
    pusha
    extern pit_tick
    call pit_tick
    mov al, 0x20
    out 0x20, al
    popa
    iretd

irq1_handler:
    pusha
    extern keyboard_isr
    call keyboard_isr
    mov al, 0x20
    out 0x20, al
    popa
    iretd
