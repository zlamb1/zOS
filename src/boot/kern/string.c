#include <stdint.h>

#include "string.h"

root_size_t
root_strlen (const char *s)
{
  root_size_t len = 0;
  while (s[len])
    len++;
  return len;
}

void
root_memcpy (void *dst, const void *src, root_size_t n)
{
  unsigned char *d = (unsigned char *) dst;
  const unsigned char *s = (const unsigned char *) src;
  while (n--)
    *d++ = *s++;
}

void
root_memmove (void *dst, const void *src, root_size_t n)
{
  unsigned char *d = (unsigned char *) dst;
  const unsigned char *s = (const unsigned char *) src;
  if ((root_uintptr_t) src < (root_uintptr_t) dst)
    {
      while (n--)
        d[n] = s[n];
    }
  else
    {
      while (n--)
        *d++ = *s++;
    }
}

void
root_memset (void *src, int v, size_t n)
{
  unsigned char *s = (unsigned char *) src;
  while (n--)
    *s++ = (unsigned char) v;
}
