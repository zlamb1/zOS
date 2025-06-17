#include "type.h"

__attribute__ ((noreturn)) void
kmain ()
{
  u8 *vmem = (u8 *) 0xB8000;

  for (u8 i = 0; i < 10; i++)
    {
      *vmem++ = 'H';
      *vmem++ = 0xFF;
    }
  for (;;)
    ;
}