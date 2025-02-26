#include "alloc/alloc.h"
#include "tui.h"

#include <stddef.h>
#include "types.h"

static struct mmap *mem_map; 

bool bmalloc_init(struct mmap *map)
{
    mem_map = map; 

    struct mm_region *region = mem_map->regions; 
    while (region != NULL && (region->type != 1 || region->base < 0x7DFF))
    {
        region++; 
    }

    if (region != NULL)
    {
        // FIXME: write allocator
    }

    return 0; 
}

void *bmalloc(u32 size)
{
    (void) size;
    return NULL;
}