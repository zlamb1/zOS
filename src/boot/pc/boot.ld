ENTRY(start)

SECTIONS
{
    STAGE1_LOC = 0x07C00;
    STAGE1_STK = 0x07C00; 
    STAGE2_LOC = 0x07E00;
    STAGE2_STK = 0x7FFFF;

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
        *(.stage2.text)
        *(.stage2.utility)
        *(.stage2.data)
        FILL(0x0)
        . = ALIGN(512);
        *(.text)
        *(.data)
        *(.bss)
        . = ALIGN(512);
        end = .;
    }

    STAGE2_SEC = (end - STAGE2_LOC) / 0x200;

    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end ) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_line_str 0 : { *(.debug_line_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
    /* DWARF 3 */
    .debug_pubtypes 0 : { *(.debug_pubtypes) }
    .debug_ranges   0 : { *(.debug_ranges) }
    /* DWARF Extension.  */
    .debug_macro    0 : { *(.debug_macro) }
    .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }

    /DISCARD/ :
    {
        *(*)
    }
}

OUTPUT_FORMAT(elf32-i386)