#ifndef TUI_H
#define TUI_H 1

#include <stdarg.h>

#include "types.h"

void tui_init(byte color);
void tui_disable_cursor(void);
void tui_enable_cursor(u8 cursor_start, u8 cursor_end);
void tui_move_cursor(u16 x, u16 y);
void tui_write_char(char c); 
u32 tui_write_str(const char *str);
u32 tui_write_int32(i32 n);
u32 tui_write_uint32(u32 n);
u32 tui_write_int64(i64 n);
u32 tui_write_uint64(u64 n);
u32 tui_write_uint32_x(u32 n);
u32 tui_write_uint64_x(u64 n);
void tui_printf(const char *fmt, ...) __attribute__((format (printf, 1, 2)));
void tui_clear(byte color);

#endif