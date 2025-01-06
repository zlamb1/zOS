.code16
.org 0

.include "defines.s"

.section .stage1, "ax"

.global _start

_start:
    # set up data segment
    mov $STAGE1_SEGMENT, %ax
    mov %ax, %ds

    # set up stack
    mov $STACK_MEM_LOC, %bp
    mov %bp, %sp

    # set up code segment
    ljmp $0, $BOOT_OFFSET

_boot:
    # store amount of low memory (KB) into %ax 
    clc
    int $0x12

    mov $0x0, %ax
    mov %ax, %es
    mov $STAGE2_MEM_LOC, %bx # %es:%bx -> 0:STAGE2_MEM_LOC

    # read two sectors (1KB)
    mov $STAGE2_NUM_SECTORS, %dh

    # load stage 2 into memory
    call chs_load

    # jump to stage 2
    ljmp $0x0, $STAGE2_MEM_LOC

/*
    %dh - num of sectors (512B) to read
    %es:%bx - data will be stored
    %dl - the drive to read from
*/
chs_load:
    pusha
    push %dx

    mov $0x02, %ah # read instruction
    mov %dh, %al # number of sectors to read
    mov $0x02, %cl # sector to read
    mov $0x0, %ch # cylinder num
    mov $0x0, %dh # head num

    clc
    int $0x13
    jc _loop

    pop %dx
    cmp %dh, %al # num sectors read is restored to dh
    jne _loop # we didn't read the num sectors requested; something failed

    popa
    ret

_loop:
    jmp _loop

.equ STAGE1_SEGMENT, STAGE1_MEM_LOC / 0x10
.equ BOOT_OFFSET, STAGE1_MEM_LOC + _boot - _start
