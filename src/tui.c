#include "tui.h"
#include "io.h"

static uint16_t VGA_WIDTH = 80, VGA_HEIGHT = 25, tui_x, tui_y; 
static char* vmem = (char*) 0xB8000;
static char active_color; 

static char hex_digits[] = {
    '0', '1', '2', '3', 
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F'
};

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
    // use VGA_HEIGHT - 1 as last row since the cursor blinking animation is invisible in the last row
    uint16_t VGA_HEIGHT_MINUS_ONE = VGA_HEIGHT - 1;
    tui_y += rows;

    if (tui_y >= VGA_HEIGHT_MINUS_ONE)
    {
        uint16_t row = (uint16_t) (tui_y - VGA_HEIGHT_MINUS_ONE + 1);
        uint16_t len = (uint16_t) (VGA_WIDTH * 2 * (VGA_HEIGHT_MINUS_ONE - row)); 
        char *prev = vmem, *cur = vmem + row * VGA_WIDTH * 2;

        // move new rows onto older ones
        while (len--)
        {
            *prev++ = *cur++;  
        }

        char *last = vmem + (VGA_HEIGHT_MINUS_ONE - 1) * VGA_WIDTH * 2;
        len = VGA_WIDTH * 2;

        // initialize new row
        while (len)
        {
            *last++ = 0x0; 
            *last++ = active_color; 
            len -= 2;
        }

        tui_y -= row;
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
            tui_x = 0;
            tui_move_cursor(tui_x, tui_y);
            break;
        case '\n':
            // line feed + carriage return
            advance_tui_y(1);
            tui_x = 0; 
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

uint32_t tui_write_str(const char *str)
{
    uint16_t pos = (uint16_t) ((tui_y * VGA_WIDTH + tui_x) * 2), len;
    while (*str != 0)
    {
        char c = *str++;

        switch (c) 
        {
            case '\r':
                tui_x = 0;
                tui_move_cursor(tui_x, tui_y);
                pos = (uint16_t) ((tui_y * VGA_WIDTH + tui_x) * 2);
                break;
            case '\n':
                advance_tui_y(1);
                tui_x = 0; 
                tui_move_cursor(tui_x, tui_y);
                pos = (uint16_t) ((tui_y * VGA_WIDTH + tui_x) * 2);
                break;
            default:
                len++;
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
    return len; 
}

uint32_t tui_write_int32(int32_t n)
{
    return tui_write_int64((int64_t) n);
}

uint32_t tui_write_uint32(uint32_t n)
{
    return tui_write_uint64((uint64_t) n);
}

uint32_t tui_write_int64(int64_t n)
{
    uint32_t neg = n < 0; 

    if (neg)
    {
        tui_write_char('-');
        n = -n;
    }

    return (uint32_t) (tui_write_uint64((uint64_t) n) + neg);
}

static inline uint32_t _write_uint64(uint64_t n)
{
    if (n == 0)
        return 0;
    
    uint64_t quotient = n / 10; 
    uint64_t remainder = n % 10; 

    uint32_t len = _write_uint64(quotient);
    tui_write_char('0' + (char) remainder);
    return len + 1;
}

uint32_t tui_write_uint64(uint64_t n)
{
    if (n == 0)
    {
        tui_write_char('0');
        return 1;
    }

    return _write_uint64(n);
}

uint32_t tui_write_uint32_x(uint32_t n)
{
    return tui_write_uint64_x(n);
}

static inline uint32_t _write_uint64_x(uint64_t n)
{
    if (n == 0)
        return 0; 

    uint64_t quotient = n / 16; 
    uint64_t remainder = n % 16; 

    uint32_t len = _write_uint64_x(quotient);
    tui_write_char(hex_digits[remainder]);
    return len + 1;
}

uint32_t tui_write_uint64_x(uint64_t n)
{
    tui_write_str("0x");

    if (n == 0)
    {
        tui_write_char('0');
        return 3;
    }

    return _write_uint64_x(n) + 2;
}

static inline uint32_t _vsprintf(const char **fmt, va_list *args)
{
    char c = *(*fmt)++;
    switch (c)
    {
        case '%':
            tui_write_char('%');
            return 1;
        case 'c':
            tui_write_char((char) va_arg(*args, int));
            return 1;
        case 'i':
        case 'd':
            return tui_write_int32(va_arg(*args, int32_t)); 
        case 'l':
            switch (*(*fmt)++)
            {
                case 'i':
                case 'd':
                    return tui_write_int32(va_arg(*args, int32_t)); 
                case 'u':
                    return tui_write_uint32(va_arg(*args, uint32_t));
                case 'l':
                    switch (*(*fmt)++)
                    {
                        case 'i':
                        case 'd':
                            return tui_write_int64(va_arg(*args, int64_t));
                        case 'u':
                            return tui_write_uint64(va_arg(*args, uint64_t));
                        case 'x':
                            return tui_write_uint64_x(va_arg(*args, uint64_t));
                        default:
                            // infinite loop to detect UB (invalid specifier)
                            for (;;);
                    }
                    break;
                case 'x':
                    return tui_write_uint32_x(va_arg(*args, uint32_t));
                default:
                    // infinite loop to detect UB (invalid specifier)
                    for (;;);
            }
            break;
        case 'p':
            tui_write_uint32(va_arg(*args, uintptr_t));
            break;
        case 's':
            tui_write_str(va_arg(*args, const char*));
            break;
        case 'u':
            tui_write_uint32(va_arg(*args, uint32_t)); 
            break;
        case 'x':
            tui_write_uint32_x(va_arg(*args, uint32_t));
            break;
        default:
        {
            // handle width specifier
            if (c == '-' || (c >= '0' && c <= '9'))
            {
                int neg = c == '-'; 
                if (neg) c = *(*fmt)++;
                uint32_t accum = 0;

                while (c >= '0' && c <= '9')
                {
                    accum *= 10;
                    accum += (uint32_t) (c - '0'); 
                    c = *(*fmt)++; 
                }

                (*fmt)--;

                if (neg)
                {
                    uint32_t len = _vsprintf(fmt, args), width = accum;
                    while (accum > len)
                    {
                        tui_write_char(' ');
                        accum--;
                    }
                    return (width > len) ? width : len;
                } else 
                {
                    va_list args_copy;
                    va_copy(args_copy, *args);
                    // copy cursor pos before printing copy
                    uint16_t _tui_x = tui_x, _tui_y = tui_y; 
                    // perform print on args_copy to determine length
                    const char *copy_fmt = *fmt; 
                    uint32_t len = _vsprintf(&copy_fmt, &args_copy), width = accum;
                    va_end(args_copy);
                    tui_x = _tui_x;
                    tui_y = _tui_y;
                    while (accum > len)
                    {
                        tui_write_char(' ');
                        accum--;
                    }
                    _vsprintf(fmt, args);
                    return (width > len) ? width : len;
                }
            }

            // infinite loop to detect UB (invalid specifier)
            for (;;);
        }
    }

    return 0; 
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
            _vsprintf(&fmt, &args);
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