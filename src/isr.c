#include "isr.h"

#include <stdint.h>

void exception_handler() 
{
    char *vmem = (char*) 0xB8000; 
    *vmem = '('; 
    for (;;);
}

static unsigned char ascii[256] =
{
    0x0, 0x0, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', 0x0, 0x0,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0x0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0x0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0x0, '*',
    0x0, ' ', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,	
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, '7',	
    '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.', 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
};

void kb_handler(struct interrupt_frame *frame)
{
    (void) frame;
    uint8_t scancode; 
    __asm__ volatile(
        "inb $0x60, %0" :
        "=r" (scancode)
    );

    if (!(scancode & 0x80))
    {
        uint8_t key = ascii[scancode];
        uint8_t *vmem = (uint8_t*) 0xB8000; 
        *vmem = key; 
    }
    
    // acknowledge PIC
    __asm__ volatile(
        "movb $0x20, %%al\n"
        "outb %%al, $0x20" : : :
        "al"
    );
}