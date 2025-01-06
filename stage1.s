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

    # store drive num
    mov %dl, (drive_number)

    # set up code segment
    ljmp $0, $BOOT_OFFSET

_boot:
    # store amount of low memory (KB) into %ax 
    clc
    int $0x12

    # FIXME: handle this
    # the interrupt failed
    jc _loop

    # load argument registers
    mov $0x0, %ax
    mov %ax, %es
    mov $STAGE2_MEM_LOC, %bx # %es:%bx -> 0:STAGE2_MEM_LOC
    mov $STAGE2_NUM_SECTORS, %dh

    # load stage 2 into memory
    call chs_load

    # jump to stage 2
    ljmp $0x0, $STAGE2_MEM_LOC

/*
    %dh - num of sectors (512B) to read
    %es:%bx - where data will be stored
    %dl - the drive to read from
*/
chs_load:
    pusha
    push %dx

    mov $0x02, %ah           # read instruction
    mov %dh, %al             # number of sectors to read
    mov $0x02, %cl           # sector to read
    mov $0x0, %ch            # cylinder num
    mov $0x0, %dh            # head num
    mov (drive_number), %dl  # drive num

    clc
    int $0x13
    jc read_fail

    pop %dx
    cmp %dh, %al       # num sectors read is restored to dh
    jne read_fail_len  # we didn't read the num sectors requested; something failed

    popa
    ret

read_fail:
    # restore stack
    pop %dx
    popa

    # save %ax
    push %ax

    # increment read_attempts
    mov (read_attempts), %al
    inc %al
    mov %al, (read_attempts)

    cmp $3, %al

    # restore %ax
    pop %ax

    # keep trying at least three times
    jb chs_load

    # print error and halt
    lea read_fail_error, %bx
    call print_str
    call wait_for_key_press
    jmp reboot

read_fail_len:
    lea read_len_error, %bx
    call print_str
    call wait_for_key_press
    jmp reboot

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

/*
    returns %al - key pressed
*/
wait_for_key_press:
    pusha
    mov $0, %ah
    int $0x16
    popa
    ret

reboot:
    # long jump to reset vector
    ljmp $0xFFFF, $0x0

_loop:
    jmp _loop

.section .stage1.data, "aw"

drive_number: .byte 0x0
read_attempts: .byte 0x0

.section .stage1.rodata, "a", @progbits

read_fail_error: .asciz "Failed to read from disk."
read_len_error: .asciz "Failed to read the required sectors from disk."

.equ STAGE1_SEGMENT, STAGE1_MEM_LOC / 0x10
.equ BOOT_OFFSET, STAGE1_MEM_LOC + _boot - _start
