#include <stdint.h>

#include "tui.h"

void loader_main(uint32_t n);

__attribute__((noreturn))
void loader_main(uint32_t n)
{
    tui_init(0x1F);
    tui_write_str("Starting zBOOT...\r\n");
    tui_write_uint32(4294967295);
    tui_write_str("\r\n");
    tui_write_uint32(n);

    for (;;);
}