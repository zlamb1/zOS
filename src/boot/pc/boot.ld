ENTRY(start)

SECTIONS
{
    STAGE1_LOC = 0x7C00;
    STAGE1_STK = 0x7C00; 
    STAGE2_LOC = 0x7E00;

    . = STAGE1_LOC;

    .stage1.text :
    {
        *(.stage1)
        *(.stage1.data)
        FILL(0x0)
        . = 510;
        SHORT(0xAA55)
    }

    .stage2.text :
    {
        *(.stage2)
        FILL(0x0)
        . = ALIGN(512);
    }
}

OUTPUT_FORMAT(elf32-i386)