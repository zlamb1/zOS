.code16

.extern _STAGE1_MEM_LOC
.extern _STAGE1_STACK_MEM_LOC
.extern _STAGE2_MEM_LOC
.extern _STAGE2_NUM_SECTORS

.section .stage1, "ax"

.global _start

#include <boot.h>

_start:
    # disable interrupts temporarily so that they don't use incorrect segments
    cli 

    # zero data and stack segment
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %ss

    # set up stack
    mov $_STAGE1_STACK_MEM_LOC, %bp
    mov %bp, %sp

    # store drive num
    mov %dl, (drive_number)

    # enable interrupts
    sti

    # set up code segment
    ljmp $0, $_boot

_boot:
    lea greeting, %si
    call print_str

    # print drive num
    lea drive_num_str, %si
    call print_str
    mov (drive_number), %al
    call print_num
    mov $'\r', %al
    call print_char
    mov $'\n', %al
    call print_char

    # load argument registers
    mov $0x0, %ax
    mov %ax, %es
    mov $_STAGE2_MEM_LOC, %bx # %es:%bx -> 0:_STAGE2_MEM_LOC
    mov $_STAGE2_NUM_SECTORS, %dh

    # load stage 2 into memory
    call chs_load
    
    # jump to stage 2
    ljmp $0x0, $_STAGE2_MEM_LOC

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
    
    # reset drive
    mov (drive_number), %dl
    mov $0, %ah
    int $0x13
    
    jc read_fail_reset

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
    lea read_fail_error, %si
    call print_str

    # check status of last operation
    mov (drive_number), %dl
    mov $0x1, %ah
    int $0x13

    # print status
    mov %ah, %al
    call print_num

    call wait_for_key_press
    jmp reboot

read_fail_len:
    lea read_len_error, %si
    call print_str
    call wait_for_key_press
    jmp reboot

read_fail_reset:
    lea read_reset_error, %si
    call print_str
    call wait_for_key_press
    jmp reboot

/*
    %al - char to print
*/
print_char:
    mov $0x0E, %ah
    int $0x10
    ret

1:
    mov $0x0E, %ah
    int $0x10
/*
    %si - address to string
*/
print_str:
    lodsb
    cmp $0, %al
    jne 1b
    ret

_print_num:
    pusha
    test %al, %al
    jz 1f

    mov $10, %bl
    mov $0, %ah
    div %bl
    call _print_num

    # store character remainder into %al
    mov $'0', %al
    add %ah, %al

    # print character
    mov $0x0E, %ah
    int $0x10

    1:
    popa
    ret

/*
    %al - number to print
*/
print_num:
    test %al, %al
    jnz 2f

    pusha
    mov $'0', %al
    call print_char
    popa
    jmp 1f

    2:
    call _print_num

    1:
    ret

/*
    returns %al - key pressed
*/
wait_for_key_press:
    mov $0, %ah
    int $0x16
    ret

reboot:
    cli
    # long jump to reset vector
    ljmp $BOOT_RESET_VECTOR, $0x0

_loop:
    jmp _loop

.section .stage1.data, "aw"

drive_number: .byte 0x0
read_attempts: .byte 0x0

.section .stage1.rodata, "a", @progbits

greeting: .asciz "Starting Stage 1...\r\n"
drive_num_str: .asciz "Drive Number: "
read_fail_error: .asciz "Disk Read Fail: "
read_len_error: .asciz "Disk Read Length Fail"
read_reset_error: .asciz "Disk Reset Fail"
