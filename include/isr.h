#ifndef ISR_H
#define ISR_H 1

struct interrupt_frame; 

void exception_handler(void) __attribute__((noreturn));
void kb_isr(struct interrupt_frame *frame) __attribute((target("general-regs-only"), interrupt));

#endif