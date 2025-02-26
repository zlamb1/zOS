#include <stdint.h>

#include "alloc/alloc.h"
#include "idt.h"
#include "kb.h"
#include "pic.h"
#include "tui.h"

extern uint32_t _STAGE2_MEM_DETAIL_LOC; 

void loader_main(void);

__attribute__((noreturn))
void loader_main()
{
    tui_init(0x1F);

    struct mmap *map = (struct mmap*) &_STAGE2_MEM_DETAIL_LOC;
    // bmalloc_init(map);

    pic_init();
    idt_init();

    tui_printf("%s\r\n", "Starting zBOOT...");

    /*MemRegion *region = regions->entries;
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
    tui_printf("\nMemory: %llxB | %lluKB | %lluMB | %lluGB\n", mem, kb, mb, gb);*/

    // event loop to handle interrupts

    while (1) 
    {
        if (has_key_event())
        {
            struct key_event event = dequeue_key_event();
            if (event.pressed && get_unicode_from_key_info(event) != '\0')
            {
                tui_write_char(get_unicode_from_key_info(event));
            }
        }
    }
}