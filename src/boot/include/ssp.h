#ifndef ROOT_SSP_H
#define ROOT_SSP_H 1

#include <stdint.h>

// TODO: randomize
extern uintptr_t __stack_chk_guard;

void __attribute__ ((noreturn)) __stack_chk_fail (void);

#endif
