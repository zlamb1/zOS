ENTRY(_entry)

SECTIONS
{
    .text :
    {
        *(.stage2)
        FILL(0x0)
        . = 512;
    }

    /DISCARD/ : { *(*) }
}

OUTPUT_FORMAT(binary)