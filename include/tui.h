#ifndef TUI_H
#define TUI_H 1

#include <stdint.h>

void init_tui(char color);
void disable_cursor(void);
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void move_cursor(uint16_t x, uint16_t y);
void advance_cursor(void);
void write_char(char c); 
void write_str(const char *str);
void clear_tui(char color);

#endif