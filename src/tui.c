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
        uint16_t row = (uint16_t) (tui_y - VGA_HEIGHT + 1);
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

void tui_init(char color)
{
    tui_clear(color);
    active_color = color;
    tui_x = 0, tui_y = 0; 
    tui_enable_cursor(0, 0);
    tui_move_cursor(tui_x, tui_y);
}

void tui_disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void tui_enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void tui_move_cursor(uint16_t x, uint16_t y)
{
	uint16_t pos = (uint16_t) ((y + 1) * VGA_WIDTH + x);

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void tui_write_char(char c)
{
    uint16_t pos = (uint16_t) ((tui_y * VGA_WIDTH + tui_x) * 2);

    switch (c)
    {
        case '\r':
            advance_tui_x(1);
            tui_x = 0;
            tui_move_cursor(tui_x, tui_y);
            break;
        case '\n':
            advance_tui_y(1);
            tui_move_cursor(tui_x, tui_y);
            break;
        default:
            vmem[pos] = c; 
            vmem[pos + 1] = active_color;
            advance_tui_x(1);
            tui_move_cursor(tui_x, tui_y);
            break;
    }
}

void tui_write_str(const char *str)
{
    uint16_t pos = (uint16_t) ((tui_y * VGA_WIDTH + tui_x) * 2);
    while (*str != 0)
    {
        char c = *str++;

        switch (c) 
        {
            case '\r':
                advance_tui_x(1);
                tui_x = 0;
                tui_move_cursor(tui_x, tui_y);
                pos = (uint16_t) ((tui_y * VGA_WIDTH + tui_x) * 2);
                break;
            case '\n':
                advance_tui_y(1);
                tui_move_cursor(tui_x, tui_y);
                pos = (uint16_t) ((tui_y * VGA_WIDTH + tui_x) * 2);
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
    tui_move_cursor(tui_x, tui_y);
}

static inline void _write_uint32(uint32_t n)
{
    if (n == 0)
        return;
    
    uint32_t quotient = n / 10; 
    uint32_t remainder = n % 10; 

    _write_uint32(quotient);
    tui_write_char('0' + (char) remainder);
}

void tui_write_int32(int32_t n)
{
    int neg = n < 0; 

    if (neg)
    {
        tui_write_char('-');
        n = -n;
    }

    tui_write_uint32((uint32_t) n);
}

void tui_write_uint32(uint32_t n)
{
    if (n == 0)
    {
        tui_write_char('0');
        return;
    }

    _write_uint32(n);
}

void tui_printf(const char *fmt, ...)
{
    va_list args; 
    va_start(args, fmt); 

    while (*fmt != '\0')
    {
        char c = *fmt++;
        if (c == '%')
        {
            switch (*fmt++)
            {
                case '%':
                    tui_write_char('%');
                    break;
                case 'c':
                    tui_write_char((char) va_arg(args, int));
                    break;
                case 'i':
                case 'd':
                    tui_write_int32(va_arg(args, int32_t)); 
                    break;
                case 's':
                    tui_write_str(va_arg(args, const char*));
                    break;
                case 'u':
                    tui_write_uint32(va_arg(args, uint32_t)); 
                    break;
                default:
                    break;
            }
        } else tui_write_char(c);
    }

    va_end(args);
}

void tui_clear(char color)
{
    int i = VGA_WIDTH * VGA_HEIGHT;
    while (i--)
    {
        int index = i * 2;
        vmem[index] = 0x0;
        vmem[index + 1] = color;
    }
}