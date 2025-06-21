#ifndef ROOT_TEXT_H
#define ROOT_TEXT_H 1

#include "types.h"

#define ROOT_TEXT_POS_RESET 0xFFFF

typedef struct root_text_pos_t
{
  root_u16 x, y;
} root_text_pos_t;

typedef struct root_text_cursor_t
{
  root_u16 x, y;
  root_u8 visible;
} root_text_cursor_t;

typedef enum root_term_type_t
{
  ROOT_TERM_TYPE_VGA,
  ROOT_TERM_TYPE_GFX
} root_term_type_t;

typedef enum root_term_scroll_type_t
{
  ROOT_SCROLL_TYPE_WRAP,
  ROOT_SCROLL_TYPE_NEWLINE
} root_term_scroll_type_t;

typedef struct rooT_term_attribs_t
{
  root_u16 tabsize;
  root_term_scroll_type_t scrolltype;
} rooT_term_attribs_t;

typedef struct root_term_t
{
  root_term_type_t type;
  rooT_term_attribs_t attribs;
  void (*putchar) (struct root_term_t *term, char c);
  void (*putchar_unsynced) (struct root_term_t *term, char c);
  root_text_pos_t (*getpos) (struct root_term_t *term);
  void (*advance) (struct root_term_t *term, root_u16 x, root_u16 y);
  void (*setpos) (struct root_term_t *term, root_u16 x, root_u16 y);
  root_text_cursor_t (*getcursor) (struct root_term_t *term);
  void (*setcursorpos) (struct root_term_t *term, root_u16 x, root_u16 y);
  void (*setcursorvisible) (struct root_term_t *term, root_u8 visible);
  void (*setcursorcolor) (struct root_term_t *term, root_u32 color);
  void (*clear) (struct root_term_t *term, root_u32 color);
} root_term_t;

extern root_term_t *root_term;

int root_terminit (root_term_t *term);
int root_inittext (void);

inline static void
root_putchar (char c)
{
  root_term->putchar (root_term, c);
}

inline static void
root_putchar_unsynced (char c)
{
  root_term->putchar_unsynced (root_term, c);
}

root_u32 root_printi32_unsynced (root_i32 n, root_u8 base, root_u8 capital);
root_u32 root_printi64_unsynced (root_i64 n, root_u8 base, root_u8 capital);
root_u32 root_printu32_unsynced (root_u32 n, root_u8 base, root_u8 capital);
root_u32 root_printu64_unsynced (root_u64 n, root_u8 base, root_u8 capital);

inline static root_u32 root_printi32 (root_i32 n, root_u8 base,
                                      root_u8 capital);
inline static root_u32 root_printi64 (root_i64 n, root_u8 base,
                                      root_u8 capital);
inline static root_u32 root_printu32 (root_u32 n, root_u8 base,
                                      root_u8 capital);
inline static root_u32 root_printu64 (root_u64 n, root_u8 base,
                                      root_u8 capital);
inline static void root_cursorsync (void);

void
root_cursorsync (void)
{
  root_text_pos_t pos = root_term->getpos (root_term);
  root_term->setcursorpos (root_term, pos.x, pos.y);
}

root_u32
root_printi32 (root_i32 n, root_u8 base, root_u8 capital)
{
  root_u32 len = root_printi32_unsynced (n, base, capital);
  root_cursorsync ();
  return len;
}

root_u32
root_printi64 (root_i64 n, root_u8 base, root_u8 capital)
{
  root_u32 len = root_printi64_unsynced (n, base, capital);
  root_cursorsync ();
  return len;
}

root_u32
root_printu32 (root_u32 n, root_u8 base, root_u8 capital)
{
  root_u32 len = root_printu32_unsynced (n, base, capital);
  root_cursorsync ();
  return len;
}

root_u32
root_printu64 (root_u64 n, root_u8 base, root_u8 capital)
{
  root_u32 len = root_printu64_unsynced (n, base, capital);
  root_cursorsync ();
  return len;
}

int __attribute__ ((format (printf, 1, 2))) root_printf (const char *fmt, ...);

#endif
