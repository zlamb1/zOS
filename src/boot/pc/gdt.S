.section .stage2.data, "aw"

.global gdt
.global gdt_desc

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
