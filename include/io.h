#ifndef IO_H
#define IO_H 1

#include "types.h"

byte inb(u16 port);
void outb(u16 port, u8 val);
void io_wait(void);

#endif