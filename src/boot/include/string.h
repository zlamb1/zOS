#ifndef ROOT_STRING_H
#define ROOT_STRING_H 1

#include "types.h"

root_size_t root_strlen (const char *s);

void root_memcpy (void *dst, const void *src, root_size_t n);
void root_memmove (void *dst, const void *src, root_size_t n);
void root_memset (void *src, int v, root_size_t n);

#endif
