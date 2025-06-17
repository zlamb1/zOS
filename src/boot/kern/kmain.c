#include "bios.h"
#include "types.h"

static const char *str = "Hello, world!";

__attribute__ ((noreturn)) void
kmain ()
{
  bios_interrupt_args args;
  u8 *vmem = (u8 *) 0xB8000;
  const char *s = str;
  for (u8 i = 0; i < 1; i++)
    {
      *vmem++ = 'H';
      *vmem++ = 0xFF;
    }
  while (*s)
    {
      args.eax = *s++ | ((0xE) << 8);
      args.ebx = 0xFF;
      bios_interrupt (0x10, &args);
    }
  for (;;)
    ;
}