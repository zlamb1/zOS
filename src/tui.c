#include "tui.h"
#include "io.h"

static uint16_t VGA_WIDTH = 80, VGA_HEIGHT = 25, tui_x, tui_y; 
static char* vmem = (char*) 0xB8000;
static char active_color; 

static inline void advance_tui_x(uint16_t cols)
{
    tui_x += cols;
    uint16_t rows = tui_x / VGA_WIDTH;

    if (rows > 0)
    {
        tui_x = tui_x % VGA_WIDTH;
        advance_tui_x(rows);
    }
}

static inline void advance_tui_y(uint16_t rows)
{
    tui_y += rows;

    if (tui_y >= VGA_HEIGHT)
    {
        uint16_t row = tui_y - VGA_HEIGHT + 1;
        uint16_t len = VGA_WIDTH * 2 * 24; 
        char *prev = vmem, *cur = vmem + row * VGA_WIDTH * 2;

        while (len--)
        {
            *prev++ = *cur++;  
        }

        char *last = vmem + (VGA_HEIGHT - 1) * VGA_WIDTH * 2;
        len = VGA_WIDTH * 2;

        while (len)
        {
            *last++ = 0x0; 
            *last++ = active_color; 
            len -= 2;
        }
    }
}

void init_tui(char color)
{
    clear_tui(color);
    active_color = color;
    tui_x = 0, tui_y = 0; 
    enable_cursor(0, 0);
    move_cursor(tui_x, tui_y);
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void move_cursor(uint16_t x, uint16_t y)
{
	uint16_t pos = (y + 1) * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void write_char(char c)
{
    uint16_t pos = (tui_y * VGA_WIDTH + tui_x) * 2;
    vmem[pos] = c; 
    vmem[pos + 1] = active_color;
    advance_tui_x(1);
    move_cursor(tui_x, tui_y);
}

void write_str(const char *str)
{
    uint16_t pos = (tui_y * VGA_WIDTH + tui_x) * 2;
    while (*str != 0)
    {
        char c = *str++;

        switch (c) 
        {
            case '\r':
                advance_tui_x(1);
                tui_x = 0;
                move_cursor(tui_x, tui_y);
                pos = (tui_y * VGA_WIDTH + tui_x) * 2;
                break;
            case '\n':
                advance_tui_y(1);
                move_cursor(tui_x, tui_y);
                pos = (tui_y * VGA_WIDTH + tui_x) * 2;
                break;
            default:
                vmem[pos] = c; 
                vmem[pos + 1] = active_color;
                advance_tui_x(1);
                if (tui_x >= VGA_WIDTH)
                {
                    tui_x = tui_x % VGA_WIDTH; 
                    tui_y++; 
                }
                pos += 2;
                break;
        }
    }
    move_cursor(tui_x, tui_y);
}

void clear_tui(char color)
{
    int i = VGA_WIDTH * VGA_HEIGHT;
    while (i--)
    {
        int index = i * 2;
        vmem[index] = 0x0;
        vmem[index + 1] = color;
    }
}