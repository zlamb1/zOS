#include <stdint.h>

#include "tui.h"

void loader_main(void);

void loader_main(void)
{
    init_tui(0x1F);
    write_str("Starting zBOOT...");

    for (;;);
}