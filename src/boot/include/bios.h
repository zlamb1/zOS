#ifndef ZBIOS_H
#define ZBIOS_H 1

#include "types.h"

typedef struct bios_interrupt_args
{
  u32 eax;
  u32 ebx;
  u32 ecx;
  u32 esi;
  u32 edi;
  u32 edx;
  u16 flags;
  u16 ds;
  u16 es;
  u16 pad[1];
} __attribute__ ((packed)) bios_interrupt_args;

void bios_interrupt (u8 intnum, bios_interrupt_args *args)
    __attribute__ ((regparm (3)));

#endif