#include "idt.h"
#include "isr.h"

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) 
{
    IDTEntry *descriptor = &idt[vector];
    descriptor->isr_low     = (uint16_t) ((uint32_t) isr & 0xFFFF);
    descriptor->kernel_cs   = 0x08; // code selector
    descriptor->attributes  = flags;
    descriptor->isr_high    = (uint16_t) ((uint32_t) isr >> 16);
    descriptor->reserved    = 0;
}

void idt_init() 
{
    idtr.base = (uintptr_t) &idt[0];
    idtr.limit = (uint16_t) sizeof(IDTEntry) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 32; vector++) 
    {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    idt_set_descriptor(33, &kb_handler, 0x8E);
    vectors[33] = true;

    __asm__ volatile("lidt %0" : : "m"(idtr)); // load the new IDT
    __asm__ volatile("sti");                   // set the interrupt flag
}