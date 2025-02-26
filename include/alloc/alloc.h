#ifndef ALLOC_H
#define ALLOC_H 1

#include <stdbool.h>

#include "types.h"

struct __attribute__((packed)) mm_region
{
    u64 base;
    u64 length;
    u32 type; 
    /* ACPI 3.0 Extended Attributes bitfield */
    u32 ACPI; 
};


struct __attribute__((packed)) mmap
{
    u32 region_count;
    byte padding[4];
    struct mm_region regions[];
};

bool bmalloc_init(struct mmap *mem_map);
void *bmalloc(u32 size); 

static inline i32 int32_alignas(i32 value, i32 alignment)
{
    // precondition: alignment is power of two
    // assumes two's complement used for negative integers
    return value + (-value & (alignment - 1)); 
}

static inline u32 uint32_alignas(u32 value, u32 alignment)
{
    // precondition: alignment is power of two
    return (value + alignment - 1) & ~(alignment - 1);
}

#endif