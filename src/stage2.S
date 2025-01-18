.code16

.extern _STAGE2_STACK_MEM_LOC
.extern _STAGE2_MEM_DETAIL_LOC

#include <boot.h>

.section .stage2, "ax"

.global _entry

_entry:
    xor %ax, %ax
    mov %ax, %ds

    lea greeting, %si
    call LOCAL(print_str)

    call LOCAL(enable_a20)

    test %ax, %ax
    jnz LOCAL(a20_enabled)

    lea a20_error, %si
    call LOCAL(print_str)
    jmp LOCAL(abort)

    LOCAL(a20_enabled):

    # store a map of memory
    call LOCAL(find_mem)
    mov (_STAGE2_MEM_DETAIL_LOC), %ax

    # disable interrupts
    cli

    # load global descriptor table
    lgdt gdt_desc

    # enter protected mode
    mov %cr0, %eax
    or CONSTANT(1), %eax
    mov %eax, %cr0

    # clear pipeline
    mov CONSTANT(LOCAL(pmode)), %ax
    ljmp CONSTANT(8), CONSTANT(LOCAL(pmode))

1:
    mov CONSTANT(0x0E), %ah
    int CONSTANT(0x10)
/*
    %si - stores address to string
*/
LOCAL(print_str):
    # clear direction flag
    cld
    lodsb
    cmp CONSTANT(0), %al
    jne 1b
    ret

LOCAL(find_mem):
    pusha

    # clear carry flag
    clc

    # setup destination (%es:%di)
    mov CONSTANT(_STAGE2_MEM_DETAIL_LOC), %di

    # add 8 bytes to preserve alignment of first entry
    add CONSTANT(8), %di

    # zero %ebx and %es
    xor %ebx, %ebx
    mov %bx, %es

    # use 4 bytes for number of entries
    mov %ebx, %es:_STAGE2_MEM_DETAIL_LOC

    # store constants
    mov CONSTANT(0x0534D4150), %edx
    mov CONSTANT(0xE820), %eax
    # force a valid ACPI 3.X entry
    mov CONSTANT(1), %ecx
    mov %ecx, %es:20(%di)
    # request 24 bytes
    mov CONSTANT(24), %ecx
    int CONSTANT(0x15)
    jc 1f
    mov CONSTANT(0x0534D4150), %edx
    cmp %edx, %eax
    jne 1f
    test %ebx, %ebx
    jz 1f
    jmp LOCAL(check_entry)

    # similar to above code
    LOCAL(read_entry):
    mov CONSTANT(0xE820), %eax
    mov CONSTANT(1), %ecx
    mov %ecx, %es:20(%di)
    mov CONSTANT(24), %ecx
    int CONSTANT(0x15)
    jc 1f
    mov CONSTANT(0x0534D4150), %edx

    LOCAL(check_entry):
    # zero bytes were read; skip entry
    jcxz LOCAL(skip_entry)
    cmp CONSTANT(20), %cl
    jbe LOCAL(notext)
    mov %es:20(%di), %eax
    test CONSTANT(1), %eax
    je LOCAL(skip_entry)

    LOCAL(notext):
    mov %es:8(%di), %ecx
    or %es:12(%di), %ecx
    jz LOCAL(skip_entry)

    # inc num_entries
    mov %es:_STAGE2_MEM_DETAIL_LOC, %eax
    inc %eax
    mov %eax, %es:_STAGE2_MEM_DETAIL_LOC
    # increment %di by 24 bytes
    add CONSTANT(24), %di

    LOCAL(skip_entry):
    test %ebx, %ebx
    jnz LOCAL(read_entry)

    1:
    clc
    popa
    ret

LOCAL(abort):
    hlt
    jmp LOCAL(abort)

#include "a20.S"

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

LOCAL(pmode):
    # set data and stack selector
    mov CONSTANT(0x10), %ax
    mov %ax, %ds
    mov %ax, %ss

    # move stack to high memory
    mov CONSTANT(_STAGE2_STACK_MEM_LOC), %ebp
    mov %ebp, %esp

    # System V ABI expects direction flag cleared
    cld
    
    mov (0x5000), %ax

    call loader_main

.include "src/isr.S"

.extern loader_main
