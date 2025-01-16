#include "kb.h"
#include "kb_def.h"

uint32_t kb_buf_size = 0, kb_buf_first = 0, kb_buf_last = 0;
uint8_t scancode_buffer[KB_BUF_SIZE];  
bool keycode_buffer[128]; 

static unsigned char scancode_to_keycode[256] =
{
    0x0, 0x0, KB_KEY_1, KB_KEY_2, KB_KEY_3, KB_KEY_4, KB_KEY_5, KB_KEY_6,
    KB_KEY_7, KB_KEY_8, KB_KEY_9, KB_KEY_0, KB_KEY_DASH, KB_KEY_EQUALS, KB_KEY_BACKSPACE, 0x0,
    KB_KEY_Q, KB_KEY_W, KB_KEY_E, KB_KEY_R, KB_KEY_T, KB_KEY_Y, KB_KEY_U, KB_KEY_I,
    KB_KEY_O, KB_KEY_P, KB_KEY_LBRACKET, KB_KEY_RBRACKET, KB_KEY_ENTER, 0x0, KB_KEY_A, KB_KEY_S,
    KB_KEY_D, KB_KEY_F, KB_KEY_G, KB_KEY_H, KB_KEY_J, KB_KEY_K, KB_KEY_L, KB_KEY_SEMICOLON,
    KB_KEY_SINGLEQUOTE, KB_KEY_BACKTICK, KB_KEY_LSHIFT, KB_KEY_BACKSLASH, KB_KEY_Z, KB_KEY_X, KB_KEY_C, KB_KEY_V,
    KB_KEY_B, KB_KEY_N, KB_KEY_M, KB_KEY_COMMA, KB_KEY_PERIOD, KB_KEY_SLASH, KB_KEY_RSHIFT, KB_KEY_ASTERISK,
    0x0, KB_KEY_SPACE, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

bool has_key_info()
{
    return kb_buf_size > 0;
}

KeyInfo dequeue_key_info()
{
    KeyInfo info = {0};
    uint8_t scancode = scancode_buffer[kb_buf_first];

    info.keycode = scancode_to_keycode[scancode & 0x7F];
    info.pressed = !(scancode & 0x80); 
    info.shift   = keycode_buffer[KB_KEY_LSHIFT] | keycode_buffer[KB_KEY_RSHIFT];
    info.alt     = keycode_buffer[KB_KEY_LALT]   | keycode_buffer[KB_KEY_RALT];
    info.ctrl    = keycode_buffer[KB_KEY_LCTRL]  | keycode_buffer[KB_KEY_RCTRL];

    keycode_buffer[info.keycode] = info.pressed;

    __asm__ volatile("cli" ::: "memory");
    kb_buf_first = (kb_buf_first + 1) % KB_BUF_SIZE; 
    kb_buf_size--;
    __asm__ volatile("sti" ::: "memory");

    return info;
}

char get_ascii_from_key_info(KeyInfo info)
{
    uint8_t keycode = info.keycode; 

    return KEYCODE_TO_UNICODE[keycode | (info.shift ? 0x80 : 0x00)];
}