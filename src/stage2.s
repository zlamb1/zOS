.code16

.extern _STAGE2_STACK_MEM_LOC

.section .stage2, "ax"

.global _entry

_entry:
    xor %ax, %ax
    mov %ax, %ds

    lea greeting, %bx
    call print_str

    call enable_a20

    test %ax, %ax
    jnz a20_enabled

    lea a20_error, %bx
    call print_str
    jmp abort

    a20_enabled:

    # disable interrupts
    cli

    # load global descriptor table
    lgdt gdt_desc

    # enter protected mode
    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0

    # clear pipeline
    mov $pmode, %ax
    ljmp $0x8, $pmode

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

abort:
    hlt
    jmp abort

.include "src/a20.s"

.section .stage2.data, "aw"

gdt:
    # null descriptor
    .quad 0x0
    gdt_code:
        # limit
        .short 0xFFFF
        # bits 1 - 16 of base address
        .short 0x0
        # bits 17 - 24 of base address
        .byte 0x0
        # type, privilege level, and present flag
        .byte 0b10011010
        # bits 17 - 20 of limit, attributes, granularity
        .byte 0b11001111
        # bits 24 - 32 of base address
        .byte 0x0
    gdt_data:
        # limit
        .short 0xFFFF
        # bits 1 - 16 of base address
        .short 0x0
        # bits 17 - 24 of base address
        .byte 0x0
        # type, privilege level, and present flag
        .byte 0b10010010
        # bits 17 - 20 of limit, attributes, granularity
        .byte 0b11001111
        # bits 24 - 32 of base address
        .byte 0x0
gdt_end:

gdt_desc:
    .short gdt_end - gdt - 1
    .long  gdt

.section .stage2.rodata, "a", @progbits

greeting: .asciz "Starting Stage 2...\r\n"
a20_error: .asciz "The A20 line could not be enabled!\r\n"

.section .stage2, "ax"
.code32

pmode:
    # set data and stack selector
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %ss

    # move stack to high memory
    mov $_STAGE2_STACK_MEM_LOC, %ebp
    mov %ebp, %esp

    # write char to screen
    movb $'P', (0xB8000)
    movb $0x1B, (0xB8001)

    jmp loader_main

.extern loader_main
