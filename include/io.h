#ifndef IO_H
#define IO_H 1

#include <stdint.h>

unsigned char inb(uint16_t port);
void outb(uint16_t port, uint8_t val);
void io_wait(void);

#endif