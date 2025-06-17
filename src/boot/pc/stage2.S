.code16

.section .stage2.text, "ax"

start2:
    call set_a20
    test %ax, %ax
    jnz 1f
    stop:
    hlt
    jmp stop
    1:
    cli
    lgdt gdt_desc
    mov %cr0, %eax
    or $0x1, %al
    mov %eax, %cr0
    ljmp $0x8, $pmode

.extern has_a20
.extern set_a20

.code32

pmode:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %ss
    cld
    jmp kmain

.extern kmain