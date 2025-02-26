#include "idt.h"
#include "isr.h"

// create an array of IDT entries; aligned for performance
static struct idt_entry idt[256] __attribute__((aligned(0x10))); 
static struct idtr idtr;
static bool vectors[IDT_MAX_DESCRIPTORS];

void idt_set_descriptor(u8 vector, void *isr, u8 flags) 
{
    struct idt_entry *descriptor = &idt[vector];
    descriptor->isr_low     = (u16) ((u32) isr & 0xFFFF);
    descriptor->kernel_cs   = 0x08; // code selector
    descriptor->attributes  = flags;
    descriptor->isr_high    = (u16) ((u32) isr >> 16);
    descriptor->reserved    = 0;
}

void idt_init() 
{
    idtr.base = (uptr) &idt[0];
    idtr.limit = (u16) sizeof(struct idt_entry) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 32; vector++) 
    {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    idt_set_descriptor(33, &kb_isr, 0x8E);
    vectors[33] = true;

    __asm__ volatile("lidt %0" : : "m"(idtr)); // load the new IDT
    __asm__ volatile("sti");                   // set the interrupt flag
}