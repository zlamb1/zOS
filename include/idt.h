#ifndef IDT_H
#define IDH_H 1

#include <stdbool.h>
#include <stdint.h>

extern void* isr_stub_table[];

#define IDT_MAX_DESCRIPTORS 256

typedef struct {
	uint16_t    isr_low;      // lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t     reserved;     // set to zero
	uint8_t     attributes;   // type and attributes; see the IDT page
	uint16_t    isr_high;     // higher 16 bits of the ISR's address
} __attribute__((packed)) IDTEntry;

typedef struct {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) IDTR;

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags);
void idt_init(void);

#endif