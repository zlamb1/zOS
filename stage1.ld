ENTRY(_start)

SECTIONS
{
    .text :
    {
        *(.stage1)
        FILL(0x0)
        . = 510;
        SHORT(0xAA55)
    }

    /DISCARD/ : { *(*) }
}

OUTPUT_FORMAT(binary)