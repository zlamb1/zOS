#ifndef IDT_H
#define IDH_H 1

#include "types.h"

extern void *isr_stub_table[];

#define IDT_MAX_DESCRIPTORS 256

struct __attribute__((packed)) idt_entry 
{
	u16    isr_low;      // lower 16 bits of the ISR's address
	u16    kernel_cs;    // GDT segment selector that the CPU will load into CS before calling the ISR
	u8     reserved;     // set to zero
	u8     attributes;   // type and attributes; see the IDT page
	u16    isr_high;     // higher 16 bits of the ISR's address
};

struct __attribute__((packed)) idtr
{
	u16	limit;
	u32	base;
};

void idt_set_descriptor(u8 vector, void *isr, u8 flags);
void idt_init(void);

#endif