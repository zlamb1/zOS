#include <stdint.h>

#include "tui.h"

void loader_main(void);

void loader_main(void)
{
    tui_init(0x1F);
    tui_write_str("Starting zBOOT...\r\n");
    tui_write_uint32(4294967295);

    for (;;);
}