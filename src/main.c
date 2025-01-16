#include <stdint.h>

#include "idt.h"
#include "kb.h"
#include "pic.h"
#include "tui.h"

extern uint32_t _STAGE2_MEM_DETAIL_LOC; 

typedef struct mem_region
{
    uint64_t base;
    uint64_t length;
    uint32_t type; 
    /* ACPI 3.0 Extended Attributes bitfield */
    uint32_t ACPI; 
} __attribute__((packed)) MemRegion;

typedef struct mem_regions
{
    uint32_t num_entries;
    char padding[4];
    MemRegion entries[];
} __attribute__((packed)) MemRegions;

void loader_main(void);

__attribute__((noreturn))
void loader_main()
{
    pic_init();
    idt_init();

    tui_init(0x1F);
    tui_printf("%s\r\n\n", "Starting zBOOT...");

    MemRegions *regions = (MemRegions*) &_STAGE2_MEM_DETAIL_LOC;

    MemRegion *region = regions->entries;
    uint64_t mem = 0;
    uint32_t len = regions->num_entries;
    while (len > 0)
    {
        if (region->type == 1)
            mem += region->length;
        tui_printf("Start=%-16llx Length=%-16llx Type=%lu\n", region->base, region->length, region->type);
        len--;
        if (len > 0) region++;
    }

    uint64_t kb = mem / 1000;
    uint64_t mb = kb / 1000; 
    uint64_t gb = mb / 1000;
    tui_printf("\nMemory: %llxB | %lluKB | %lluMB | %lluGB\n", mem, kb, mb, gb);

    w:
    while (!has_key_info());
    
    KeyInfo info = dequeue_key_info();
    char c = get_ascii_from_key_info(info);
    if (info.pressed && c != '\0')
    {
        tui_write_char(get_ascii_from_key_info(info));
    }

    goto w;
}