#ifndef TUI_H
#define TUI_H 1

#include <stdint.h>

void tui_init(char color);
void tui_disable_cursor(void);
void tui_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void tui_move_cursor(uint16_t x, uint16_t y);
void tui_write_char(char c); 
void tui_write_str(const char *str);
void tui_write_uint32(uint32_t n);
void tui_clear(char color);

#endif