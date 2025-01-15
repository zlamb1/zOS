#include "isr.h"
#include "kb.h"

#include <stdint.h>

void exception_handler() 
{
    char *vmem = (char*) 0xB8000; 
    *vmem = '('; 
    for (;;);
}

void kb_handler(struct interrupt_frame *frame)
{
    (void) frame;
    uint8_t scancode; 

    __asm__ volatile(
        "inb $0x60, %0" :
        "=r" (scancode)
    );
    
    if (kb_buf_size < KB_BUF_SIZE)
    {
        if (kb_buf_size == 0)
        {
            kb_buf_first = 0;
            kb_buf_last  = 0;
            scancode_buffer[0] = scancode; 
        } else
        {
            kb_buf_last = (kb_buf_last + 1) % KB_BUF_SIZE;
            scancode_buffer[kb_buf_last] = scancode; 
        }
        kb_buf_size++;
    }

    // acknowledge PIC
    __asm__ volatile(
        "movb $0x20, %%al\n"
        "outb %%al, $0x20" : : :
        "al"
    );
}