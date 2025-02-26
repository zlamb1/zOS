#include "io.h"

byte inb(u16 port)
{
    byte out;

    __asm__ volatile(
        "inb %w1, %b0" :
        "=a"(out)      :
        "Nd"(port)     : 
        "memory"
    );

    return out;
}

void outb(u16 port, u8 val)
{
    __asm__ __volatile__(
        "outb %b0, %w1"        : 
                               :
        "a" (val), "Nd" (port) :
        "memory" 
    );
}

void io_wait()
{
    outb(0x80, 0);
}