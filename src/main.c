#include <stdint.h>

#include "tui.h"

void loader_main(void);

__attribute__((noreturn))
void loader_main()
{
    tui_init(0x1F);
    tui_printf("%s\r\n", "Starting zBOOT...");

    for (;;);
}