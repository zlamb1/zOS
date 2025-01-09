ENTRY(_entry)

SECTIONS
{
    _STAGE1_MEM_LOC = 0x7C00;
    _STAGE1_STACK_MEM_LOC = 0x9000;
    _STAGE2_MEM_LOC = 0x1000;
    _STAGE2_STACK_MEM_LOC = 0x90000;

    . = _STAGE1_MEM_LOC;

    /* Linker generates warning: <file> has a LOAD segment with RWX permissions */
    /* This can be safely ignored and is caused by text and data sections not being page-aligned. */
    .stage1.text :
    {
        *(.stage1)   
        *(.stage1.data)
        *(.stage1.rodata)
        
        FILL(0x0)
        . = 510;
        SHORT(0xAA55) 
    }

    .stage2.text _STAGE2_MEM_LOC : AT(_STAGE1_MEM_LOC + 0x200)
    {
        *(.stage2)
        . = ALIGN(32);
        *(.stage2.data)
        *(.stage2.rodata)

        /* pad such that C code is at static offset from stage 2  */
        FILL(0x0)
        . = 512;

        /* used by C compiler */
        *(.text)
        *(.data)
        *(.rodata)
        _bss_start = .;
        *(.bss)
        *(COMMON*)
        _bss_end   = .;
        *(.ctors)
        *(.dtors)
        *(.eh_frame)
        *(.tm_clone_table)

        /* align to 512 bytes */
        FILL(0x0)
        . = ALIGN(512);
        _end = .;
    }

    /* size of stage2 divided by 512 */
    _STAGE2_NUM_SECTORS = (_end - _STAGE2_MEM_LOC) / 0x200; 

    /DISCARD/ : { *(*) }
}

OUTPUT_FORMAT(elf32-i386)