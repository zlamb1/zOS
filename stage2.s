.code16
.org 0

.include "defines.s"

.section .stage2, "ax"

.global _entry

_entry:
    mov $STAGE2_DS, %ax
    mov %ax, %ds
    lea greeting, %bx
    call print_str
    jmp _loop

/*
    %bx - stores address to string
*/
print_str:
    pusha

    # clear direction flag
    cld
    mov %bx, %si
    
    write:
    lodsb

    cmp $0, %al
    je end

    mov $0x0E, %ah
    int $0x10
    jmp write

    end:
    popa
    ret

_loop:
    jmp _loop

greeting: .asciz "Welcome to Stage 2!"

.equ STAGE2_DS, STAGE2_MEM_LOC / 0x10
