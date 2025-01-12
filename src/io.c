#include "io.h"

unsigned char inb(uint16_t port)
{
    unsigned char out;

    __asm__ volatile(
        "inb %w1, %b0" :
        "=a"(out)      :
        "Nd"(port)     : 
        "memory"
    );

    return out;
}

void outb(uint16_t port, uint8_t val)
{
    __asm__ __volatile__(
        "outb %b0, %w1"        : 
                               :
        "a" (val), "Nd" (port) :
        "memory" 
    );
}

void io_wait(void)
{
    outb(0x80, 0);
}