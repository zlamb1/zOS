.equ BOOT_SIG_LOC, _STAGE1_MEM_LOC + 0x1FE
.equ WRAP_OFFSET, BOOT_SIG_LOC + 0x10

#include <boot.h>

/*
    returns %ax - whether a20 is enabled
*/
LOCAL(check_a20):
    pusha

    mov CONSTANT(0), %ax
    mov %ax, %es
    mov CONSTANT(0xFFFF), %ax
    mov %ax, %fs

    mov %es:BOOT_SIG_LOC, %ax
    mov %fs:WRAP_OFFSET, %bx

    cmp %ax, %bx
    jne 1f

    # rotate %ax left by 8-bits
    rol CONSTANT(8), %ax

    mov %ax, %es:BOOT_SIG_LOC
    mov %fs:WRAP_OFFSET, %bx

    cmp %ax, %bx

    # restore the boot signature
    rol CONSTANT(8), %ax
    mov %ax, %es:BOOT_SIG_LOC

    jne 1f

    popa
    mov CONSTANT(0), %ax
    ret

    1:
    popa
    mov CONSTANT(1), %ax
    ret

/*
    returns %ax - whether a20 was successfully disabled
*/
LOCAL(disable_a20):
    pusha
    mov CONSTANT(0x2400), %ax
    int CONSTANT(0x15)
    popa

    call LOCAL(check_a20)

    1:
    # invert %ax
    test %ax, %ax
    setz %al

    ret

/*
    returns %ax - whether a20 was successfully enabled
*/
LOCAL(enable_a20):
    pusha

    call LOCAL(check_a20)
    cmp CONSTANT(1), %ax
    test %ax, %ax
    # a20 is already enabled, do nothing!
    jnz 1f

    # clear carry flag
    clc

    # query A20 support
    mov CONSTANT(0x2403), %ax
    int CONSTANT(0x15)

    # int 0x15 not supported
    jc 2f

    # A20 not supported
    test %ah, %ah
    jnz 2f

    # set gate enabled
    mov CONSTANT(0x2401), %ax
    int CONSTANT(0x15) 

    # interrupt failed
    jc 2f

    # check result for safety
    call LOCAL(check_a20)
    test %ax, %ax
    jnz 1f

    2:

    # read ioport 0xEE to try and enable A20
    in CONSTANT(0xEE), %al

    # check result
    call LOCAL(check_a20)
    test %ax, %ax
    jnz 1f

    # fast A20
    in CONSTANT(0x92), %al
    or CONSTANT(2), %al
    out %al, $0x92
    
    # waste clock cycles waiting for fast A20 to work
    mov CONSTANT(0), %ax
    LOCAL(loop):
    inc %ax
    cmp CONSTANT(100), %ax
    jb LOCAL(loop)

    # check result
    call LOCAL(check_a20)
    test %ax, %ax
    jnz 1f

    # FIXME: implement keyboard controlller method

    popa 
    mov CONSTANT(0), %ax
    ret

    1:
    popa
    mov CONSTANT(1), %ax
    ret
