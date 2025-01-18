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
    mov CONSTANT(_STAGE1_STACK_MEM_LOC), %bp
    mov %bp, %sp

    # store drive num
    mov %dl, (drive_number)

    # enable interrupts
    sti

    # set up code segment
    ljmp CONSTANT(0), CONSTANT(LOCAL(_boot))

LOCAL(_boot):
    lea greeting, %si
    call LOCAL(print_str)

    # print drive num
    lea drive_num_str, %si
    call LOCAL(print_str)
    mov (drive_number), %al
    call LOCAL(print_num)
    mov CONSTANT('\r'), %al
    call LOCAL(print_char)
    mov CONSTANT('\n'), %al
    call LOCAL(print_char)

    # load argument registers
    mov CONSTANT(0), %ax
    mov %ax, %es
    mov CONSTANT(_STAGE2_MEM_LOC), %bx # %es:%bx -> 0:_STAGE2_MEM_LOC
    mov CONSTANT(_STAGE2_NUM_SECTORS), %dh

    # load stage 2 into memory
    call LOCAL(chs_load)

    # jump to stage 2
    ljmp CONSTANT(0), CONSTANT(_STAGE2_MEM_LOC)

/*
    %dh - num of sectors (512B) to read
    %es:%bx - where data will be stored
    %dl - the drive to read from
*/
LOCAL(chs_load):
    pusha
    push %dx

    mov CONSTANT(2), %ah           # read instruction
    mov %dh, %al             # number of sectors to read
    mov CONSTANT(2), %cl           # sector to read
    mov CONSTANT(0), %ch            # cylinder num
    mov CONSTANT(0), %dh            # head num
    mov (drive_number), %dl  # drive num

    clc
    int CONSTANT(0x13)
    jc LOCAL(read_fail)

    pop %dx
    # num sectors read is restored to dh
    cmp %dh, %al       
    # we didn't read the num sectors requested; something failed
    jne LOCAL(read_fail_len)

    popa
    ret

LOCAL(read_fail):
    # restore stack
    pop %dx
    popa

    # save %ax
    push %ax
    
    # reset drive
    mov (drive_number), %dl
    mov CONSTANT(0), %ah
    int CONSTANT(0x13)
    
    jc LOCAL(read_fail_reset)

    # increment read_attempts
    mov (read_attempts), %al
    inc %al
    mov %al, (read_attempts)

    cmp CONSTANT(3), %al

    # restore %ax
    pop %ax

    # keep trying at least three times
    jb LOCAL(chs_load)

    # print error and halt
    lea read_fail_error, %si
    call LOCAL(print_str)

    # check status of last operation
    mov (drive_number), %dl
    mov CONSTANT(0x1), %ah
    int CONSTANT(0x13)

    # print status
    mov %ah, %al
    call LOCAL(print_num)

    call LOCAL(read_input)
    jmp LOCAL(reboot)

LOCAL(read_fail_len):
    lea read_len_error, %si
    call LOCAL(print_str)
    call LOCAL(read_input)
    jmp LOCAL(reboot)

LOCAL(read_fail_reset):
    lea read_reset_error, %si
    call LOCAL(print_str)
    call LOCAL(read_input)
    jmp LOCAL(reboot)

/*
    %al - char to print
*/
LOCAL(print_char):
    mov CONSTANT(0x0E), %ah
    int CONSTANT(0x10)
    ret

1:
    mov CONSTANT(0x0E), %ah
    int CONSTANT(0x10)
/*
    %si - address to string
*/
LOCAL(print_str):
    cld
    lodsb
    cmp CONSTANT(0), %al
    jne 1b
    ret

LOCAL(_print_num):
    pusha
    test %al, %al
    jz 1f

    mov CONSTANT(10), %bl
    mov CONSTANT(0), %ah
    div %bl
    call LOCAL(_print_num)

    # store character remainder into %al
    mov CONSTANT('0'), %al
    add %ah, %al

    # print character
    mov CONSTANT(0x0E), %ah
    int CONSTANT(0x10)

    1:
    popa
    ret

/*
    %al - number to print
*/
LOCAL(print_num):
    test %al, %al
    jnz 2f

    pusha
    mov CONSTANT('0'), %al
    call LOCAL(print_char)
    popa
    jmp 1f

    2:
    call LOCAL(_print_num)

    1:
    ret

/*
    returns %al - key pressed
*/
LOCAL(read_input):
    mov CONSTANT(0), %ah
    int CONSTANT(0x16)
    ret

LOCAL(reboot):
    cli
    # long jump to reset vector
    ljmp CONSTANT(BOOT_RESET_VECTOR), CONSTANT(0)

.section .stage1.data, "aw"

drive_number: .byte 0x0
read_attempts: .byte 0x0

.section .stage1.rodata, "a", @progbits

greeting: .asciz "Starting Stage 1...\r\n"
drive_num_str: .asciz "Drive Number: "
read_fail_error: .asciz "Disk Read Fail: "
read_len_error: .asciz "Disk Read Length Fail"
read_reset_error: .asciz "Disk Reset Fail"
