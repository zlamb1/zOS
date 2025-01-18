.equ BOOT_SIG_LOC, _STAGE1_MEM_LOC + 0x1FE
.equ WRAP_OFFSET, BOOT_SIG_LOC + 0x10

/*
    returns %ax - whether a20 is enabled
*/
check_a20:
    pusha

    mov $0, %ax
    mov %ax, %es
    mov $0xFFFF, %ax
    mov %ax, %fs

    mov %es:BOOT_SIG_LOC, %ax
    mov %fs:WRAP_OFFSET, %bx

    cmp %ax, %bx
    jne 1f

    # rotate %ax left by 8-bits
    rol $8, %ax

    mov %ax, %es:BOOT_SIG_LOC
    mov %fs:WRAP_OFFSET, %bx

    cmp %ax, %bx

    # restore the boot signature
    rol $8, %ax
    mov %ax, %es:BOOT_SIG_LOC

    jne 1f

    popa
    mov $0, %ax
    ret

    1:
    popa
    mov $1, %ax
    ret

/*
    returns %ax - whether a20 was successfully disabled
*/
disable_a20:
    pusha
    mov $0x2400, %ax
    int $0x15
    popa

    call check_a20

    1:
    # invert %ax
    test %ax, %ax
    setz %al

    ret

/*
    returns %ax - whether a20 was successfully enabled
*/
enable_a20:
    pusha

    # if a20 is already enabled, do nothing!
    call check_a20
    cmp $1, %ax
    test %ax, %ax
    jnz 1f

    # clear carry flag
    clc

    # query A20 support
    mov $0x2403, %ax
    int $0x15

    # int 0x15 not supported
    jc 2f

    # A20 not supported
    test %ah, %ah
    jnz 2f

    # set gate enabled
    mov $0x2401, %ax
    int $0x15 

    # interrupt failed
    jc 2f

    # check result for safety
    call check_a20
    test %ax, %ax
    jnz 1f

    2:

    # read ioport 0xEE to try and enable A20
    in $0xEE, %al

    # check result
    call check_a20
    test %ax, %ax
    jnz 1f

    # fast A20
    in $0x92, %al
    or $2, %al
    out %al, $0x92
    
    # waste clock cycles waiting for fast A20 to work
    mov $0, %ax
    Lloop:
    inc %ax
    cmp $100, %ax
    jb Lloop

    # check result
    call check_a20
    test %ax, %ax
    jnz 1f

    # FIXME: implement keyboard controlller method

    popa 
    mov $0, %ax
    ret

    1:
    popa
    mov $1, %ax
    ret
