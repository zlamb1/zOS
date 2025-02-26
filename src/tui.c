#include "tui.h"
#include "io.h"

static u16 VGA_WIDTH = 80, VGA_HEIGHT = 25, tui_x, tui_y; 
static char* vmem = (char*) 0xB8000;
static char active_color; 

static char hex_digits[] = {
    '0', '1', '2', '3', 
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F'
};

static inline void advance_tui_y(u16 rows)
{
    // use VGA_HEIGHT - 1 as last row since the cursor blinking animation is invisible in the last row
    u16 VGA_HEIGHT_MINUS_ONE = VGA_HEIGHT - 1;
    tui_y += rows;

    if (tui_y >= VGA_HEIGHT_MINUS_ONE)
    {
        u16 row = (u16) (tui_y - VGA_HEIGHT_MINUS_ONE + 1);
        u16 len = (u16) (VGA_WIDTH * 2 * (VGA_HEIGHT_MINUS_ONE - row)); 
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

static inline void advance_tui_x(u16 cols)
{
    tui_x += cols;
    u16 rows = tui_x / VGA_WIDTH;

    if (rows > 0)
    {
        tui_x = tui_x % VGA_WIDTH;
        advance_tui_y(rows);
    }
}

void tui_init(byte color)
{
    tui_clear(color);
    active_color = (char) color;
    tui_x = 0, tui_y = 0; 
    tui_enable_cursor(0, 0);
    tui_move_cursor(tui_x, tui_y);
}

void tui_disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void tui_enable_cursor(u8 cursor_start, u8 cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void tui_move_cursor(u16 x, u16 y)
{
	u16 pos = (u16) ((y + 1) * VGA_WIDTH + x);

	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8) ((pos >> 8) & 0xFF));
}

static inline void _write_char_at_pos(char c, u16 pos)
{
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
        case '\b':
            if (tui_x > 0)
            {
                tui_x--;
                pos -= 2; 
                vmem[pos] = '\0'; 
                vmem[pos + 1] = active_color;
                tui_move_cursor(tui_x, tui_y);
            }
            break;
        default:
            vmem[pos] = c; 
            vmem[pos + 1] = active_color;
            advance_tui_x(1);
            tui_move_cursor(tui_x, tui_y);
            break;
    }
} 

void tui_write_char(char c)
{
    _write_char_at_pos(c, (u16) ((tui_y * VGA_WIDTH + tui_x) * 2));
}

u32 tui_write_str(const char *str)
{
    u16 pos = (u16) ((tui_y * VGA_WIDTH + tui_x) * 2), len;
    while (*str != 0)
    {
        char c = *str++;

        switch (c) 
        {
            case '\r':
                tui_x = 0;
                tui_move_cursor(tui_x, tui_y);
                pos = (u16) ((tui_y * VGA_WIDTH + tui_x) * 2);
                break;
            case '\n':
                advance_tui_y(1);
                tui_x = 0; 
                tui_move_cursor(tui_x, tui_y);
                pos = (u16) ((tui_y * VGA_WIDTH + tui_x) * 2);
                break;
            case '\b':
                if (tui_x > 0)
                {
                    tui_x--;
                    pos -= 2; 
                    vmem[pos] = '\0'; 
                    vmem[pos + 1] = active_color;
                }
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

u32 tui_write_int32(i32 n)
{
    return tui_write_int64((int64_t) n);
}

u32 tui_write_uint32(u32 n)
{
    return tui_write_uint64((uint64_t) n);
}

u32 tui_write_int64(i64 n)
{
    u32 neg = n < 0; 

    if (neg)
    {
        tui_write_char('-');
        n = -n;
    }

    return (u32) (tui_write_uint64((u64) n) + neg);
}

static inline u32 _write_uint64(u64 n)
{
    if (n == 0)
        return 0;
    
    u64 quotient = n / 10; 
    u64 remainder = n % 10; 

    u32 len = _write_uint64(quotient);
    tui_write_char('0' + (char) remainder);
    return len + 1;
}

u32 tui_write_uint64(u64 n)
{
    if (n == 0)
    {
        tui_write_char('0');
        return 1;
    }

    return _write_uint64(n);
}

u32 tui_write_uint32_x(u32 n)
{
    return tui_write_uint64_x(n);
}

static inline u32 _write_uint64_x(u64 n)
{
    if (n == 0)
        return 0; 

    u64 quotient = n / 16; 
    u64 remainder = n % 16; 

    u32 len = _write_uint64_x(quotient);
    tui_write_char(hex_digits[remainder]);
    return len + 1;
}

u32 tui_write_uint64_x(u64 n)
{
    tui_write_str("0x");

    if (n == 0)
    {
        tui_write_char('0');
        return 3;
    }

    return _write_uint64_x(n) + 2;
}

static inline u32 _vsprintf(const char **fmt, va_list *args)
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
            return tui_write_int32(va_arg(*args, i32)); 
        case 'l':
            switch (*(*fmt)++)
            {
                case 'i':
                case 'd':
                    return tui_write_int32(va_arg(*args, i32)); 
                case 'u':
                    return tui_write_uint32(va_arg(*args, u32));
                case 'l':
                    switch (*(*fmt)++)
                    {
                        case 'i':
                        case 'd':
                            return tui_write_int64(va_arg(*args, i64));
                        case 'u':
                            return tui_write_uint64(va_arg(*args, u64));
                        case 'x':
                            return tui_write_uint64_x(va_arg(*args, u64));
                        default:
                            // infinite loop to detect UB (invalid specifier)
                            for (;;);
                    }
                    break;
                case 'x':
                    return tui_write_uint32_x(va_arg(*args, u32));
                default:
                    // infinite loop to detect UB (invalid specifier)
                    for (;;);
            }
            break;
        case 'p':
            tui_write_uint32(va_arg(*args, uptr));
            break;
        case 's':
            tui_write_str(va_arg(*args, const char*));
            break;
        case 'u':
            tui_write_uint32(va_arg(*args, u32)); 
            break;
        case 'x':
            tui_write_uint32_x(va_arg(*args, u32));
            break;
        default:
        {
            // handle width specifier
            if (c == '-' || (c >= '0' && c <= '9'))
            {
                int neg = c == '-'; 
                if (neg) c = *(*fmt)++;
                u32 accum = 0;

                while (c >= '0' && c <= '9')
                {
                    accum *= 10;
                    accum += (u32) (c - '0'); 
                    c = *(*fmt)++; 
                }

                (*fmt)--;

                if (neg)
                {
                    u32 len = _vsprintf(fmt, args), width = accum;
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
                    u16 _tui_x = tui_x, _tui_y = tui_y; 
                    // perform print on args_copy to determine length
                    const char *copy_fmt = *fmt; 
                    u32 len = _vsprintf(&copy_fmt, &args_copy), width = accum;
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

void tui_clear(byte color)
{
    int i = VGA_WIDTH * VGA_HEIGHT;
    while (i--)
    {
        int index = i * 2;
        vmem[index] = 0x0;
        vmem[index + 1] = (char) color;
    }
}