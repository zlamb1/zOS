.code16

.section .stage2, "ax"

start2:
    mov $0xE, %ah
    mov $'H', %al
    mov $0x0, %bh
    int $0x10
    loop:
    hlt
    jmp loop