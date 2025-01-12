.code16

.extern _STAGE2_STACK_MEM_LOC
.extern _STAGE2_MEM_DETAIL_LOC

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

    # store a map of memory
    call find_mem
    mov (_STAGE2_MEM_DETAIL_LOC), %ax

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

find_mem:
    pusha

    # clear carry flag
    clc

    # setup destination (%es:%di)
    mov $_STAGE2_MEM_DETAIL_LOC, %di

    # add 8 bytes to preserve alignment of first entry
    add $8, %di

    # zero %ebx and %es
    xor %ebx, %ebx
    mov %bx, %es

    # use 4 bytes for number of entries
    mov %ebx, %es:_STAGE2_MEM_DETAIL_LOC

    # store constants
    mov $0x0534D4150, %edx
    mov $0xE820, %eax
    # force a valid ACPI 3.X entry
    mov $1, %ecx
    mov %ecx, %es:20(%di)
    # request 24 bytes
    mov $24, %ecx
    int $0x15
    jc 1f
    mov $0x0534D4150, %edx
    cmp %edx, %eax
    jne 1f
    test %ebx, %ebx
    jz 1f
    jmp check_entry

    # similar to above code
    read_entry:
    mov $0xE820, %eax
    mov $1, %ecx
    mov %ecx, %es:20(%di)
    mov $24, %ecx
    int $0x15
    jc 1f
    mov $0x0534D4150, %edx

    check_entry:
    # if zero bytes were read skip entry
    jcxz skip_entry
    cmp $20, %cl
    jbe notext
    mov %es:20(%di), %eax
    test $1, %eax
    je skip_entry

    notext:
    mov %es:8(%di), %ecx
    or %es:12(%di), %ecx
    jz skip_entry

    # inc num_entries
    mov %es:_STAGE2_MEM_DETAIL_LOC, %eax
    inc %eax
    mov %eax, %es:_STAGE2_MEM_DETAIL_LOC
    # increment %di by 24 bytes
    add $24, %di

    skip_entry:
    test %ebx, %ebx
    jnz read_entry

    1:
    clc
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

    # System V ABI expects direction flag cleared
    cld
    
    mov (0x5000), %ax

    call loader_main

.include "src/isr.s"

.extern loader_main
