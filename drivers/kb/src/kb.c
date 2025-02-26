#include "kb.h"
#include "kb_def.h"

u32 kb_buf_size = 0, kb_buf_first = 0, kb_buf_last = 0;
u8 scancode_buffer[KB_BUF_SIZE];  
bool keycode_buffer[128];
bool capslock_active = false;

static unsigned char scancode_to_keycode[256] =
{
    0x0, 0x0, KB_KEY_1, KB_KEY_2, KB_KEY_3, KB_KEY_4, KB_KEY_5, KB_KEY_6,
    KB_KEY_7, KB_KEY_8, KB_KEY_9, KB_KEY_0, KB_KEY_DASH, KB_KEY_EQUALS, KB_KEY_BACKSPACE, 0x0,
    KB_KEY_Q, KB_KEY_W, KB_KEY_E, KB_KEY_R, KB_KEY_T, KB_KEY_Y, KB_KEY_U, KB_KEY_I,
    KB_KEY_O, KB_KEY_P, KB_KEY_LBRACKET, KB_KEY_RBRACKET, KB_KEY_ENTER, 0x0, KB_KEY_A, KB_KEY_S,
    KB_KEY_D, KB_KEY_F, KB_KEY_G, KB_KEY_H, KB_KEY_J, KB_KEY_K, KB_KEY_L, KB_KEY_SEMICOLON,
    KB_KEY_SINGLEQUOTE, KB_KEY_BACKTICK, KB_KEY_LSHIFT, KB_KEY_BACKSLASH, KB_KEY_Z, KB_KEY_X, KB_KEY_C, KB_KEY_V,
    KB_KEY_B, KB_KEY_N, KB_KEY_M, KB_KEY_COMMA, KB_KEY_PERIOD, KB_KEY_SLASH, KB_KEY_RSHIFT, KB_KEY_ASTERISK,
    0x0, KB_KEY_SPACE, KB_KEY_CAPSLOCK, 0x0, 0x0, 0x0, 0x0, 0x0
};

bool has_key_event()
{
    return kb_buf_size > 0;
}

struct key_event dequeue_key_event()
{
    struct key_event event = {0};
    u8 scancode = scancode_buffer[kb_buf_first];

    event.keycode = scancode_to_keycode[scancode & 0x7F];
    event.pressed = !(scancode & 0x80); 
    event.shift   = keycode_buffer[KB_KEY_LSHIFT] | keycode_buffer[KB_KEY_RSHIFT];
    event.ctrl    = keycode_buffer[KB_KEY_LCTRL]  | keycode_buffer[KB_KEY_RCTRL];
    event.caps    = capslock_active;
    event.alt     = keycode_buffer[KB_KEY_LALT]   | keycode_buffer[KB_KEY_RALT];
    
    keycode_buffer[event.keycode] = event.pressed;

    if (event.keycode == KB_KEY_CAPSLOCK && event.pressed)
        capslock_active = !capslock_active;

    __asm__ volatile("cli" ::: "memory");
    kb_buf_first = (kb_buf_first + 1) % KB_BUF_SIZE; 
    kb_buf_size--;
    __asm__ volatile("sti" ::: "memory");

    return event;
}

char get_unicode_from_key_info(struct key_event event)
{
    u8 keycode = event.keycode; 
    char unmodified_unicode = KEYCODE_TO_UNICODE[keycode];

    if (unmodified_unicode >= 'a' && unmodified_unicode <= 'z')
    {
        // unsigned overflow allows us to wrap to the correct index
        u8 index = keycode + (u8) ((event.shift ? 0x80 : 0x00) + (event.caps ? 0x80 : 0x00));
        return KEYCODE_TO_UNICODE[index];
    }

    return KEYCODE_TO_UNICODE[keycode | (event.shift ? 0x80 : 0x00)];
}