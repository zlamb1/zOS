ENTRY(_start)

SECTIONS
{
    .text :
    {
        *(.stage1)
        *(.stage1.data)
        *(.stage1.rodata)
        FILL(0x0)
        . = 510;
        SHORT(0xAA55)
    }

    /DISCARD/ : { *(*) }
}

OUTPUT_FORMAT(binary)