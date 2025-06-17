.code16

.extern STAGE1_STK
.extern STAGE2_LOC
.extern STAGE2_SEC

.section .stage1.text, "ax"

.global start

start:
    cli
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %ss
    mov $STAGE1_STK, %ax
    mov %ax, %sp
    ljmp $0, $boot

boot:
    sti
    mov $0x41, %ah
    mov $0x55AA, %bx
    mov $0x80, %dl
    int $0x13
    mov $0x42, %ah
    mov $disk_addr_packet, %si
    mov $0x80, %dl
    int $0x13
    ljmp $0, $STAGE2_LOC

.section .stage1.data, "aw"

disk_addr_packet:
.align 4
.byte  0x0010
.byte  0x0000
.short STAGE2_SEC
.short STAGE2_LOC
.short 0x0000
.word  0x0001
.word  0x0000