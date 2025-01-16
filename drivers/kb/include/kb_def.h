#ifndef KB_DEF
#define KB_DEF 1

#define KB_KEY_ESCAPE       0x00
#define KB_KEY_F1           0x01
#define KB_KEY_F2           0x02
#define KB_KEY_F3           0x03
#define KB_KEY_F4           0x04
#define KB_KEY_F5           0x05
#define KB_KEY_F6           0x06
#define KB_KEY_F7           0x07
#define KB_KEY_F8           0x08
#define KB_KEY_F9           0x09
#define KB_KEY_F10          0x0A
#define KB_KEY_F11          0x0B
#define KB_KEY_F12          0x0C
#define KB_KEY_BACKTICK     0x0D
#define KB_KEY_TILDE        KB_KEY_BACKTICK
#define KB_KEY_0            0x0E
#define KB_KEY_ZERO         KB_KEY_0
#define KB_KEY_RPAREN       KB_KEY_0
#define KB_KEY_1            0x0F
#define KB_KEY_ONE          KB_KEY_1
#define KB_KEY_EXCLAM       KB_KEY_1
#define KB_KEY_2            0x10
#define KB_KEY_TWO          KB_KEY_2
#define KB_KEY_AT           KB_KEY_2
#define KB_KEY_3            0x11
#define KB_KEY_THREE        KB_KEY_3
#define KB_KEY_HASH         KB_KEY_3
#define KB_KEY_4            0x12
#define KB_KEY_FOUR         KB_KEY_4
#define KB_KEY_DOLLAR       KB_KEY_4
#define KB_KEY_5            0x13
#define KB_KEY_FIVE         KB_KEY_5
#define KB_KEY_PERCENT      KB_KEY_5
#define KB_KEY_6            0x14
#define KB_KEY_SIX          KB_KEY_6
#define KB_KEY_CARET        KB_KEY_6
#define KB_KEY_7            0x15
#define KB_KEY_SEVEN        KB_KEY_7
#define KB_KEY_AMPERSAND    KB_KEY_7
#define KB_KEY_8            0x16
#define KB_KEY_EIGHT        KB_KEY_8
#define KB_KEY_ASTERISK     KB_KEY_8
#define KB_KEY_9            0x17
#define KB_KEY_NINE         KB_KEY_9
#define KB_KEY_LPAREN       KB_KEY_9
#define KB_KEY_DASH         0x18
#define KB_KEY_UNDERSCORE   KB_KEY_DASH
#define KB_KEY_EQUALS       0x19
#define KB_KEY_PLUS         KB_KEY_EQUALS
#define KB_KEY_BACKSPACE    0x1A

#define KB_KEY_TAB          0x1B
#define KB_KEY_Q            0x1C
#define KB_KEY_W            0x1D
#define KB_KEY_E            0x1E
#define KB_KEY_R            0x1F
#define KB_KEY_T            0x20
#define KB_KEY_Y            0x21
#define KB_KEY_U            0x22
#define KB_KEY_I            0x23
#define KB_KEY_O            0x24
#define KB_KEY_P            0x25
#define KB_KEY_LBRACKET     0x26
#define KB_KEY_LBRACE       KB_KEY_LBRACKET
#define KB_KEY_RBRACKET     0x27
#define KB_KEY_RBRACE       KB_KEY_RBRACKET
#define KB_KEY_BACKSLASH    0x28
#define KB_KEY_PIPE         KB_KEY_BACKSLASH
#define KB_KEY_VERTICALBAR  KB_KEY_BACKSLASH

#define KB_KEY_CAPSLOCK     0x29
#define KB_KEY_A            0x2A
#define KB_KEY_S            0x2B
#define KB_KEY_D            0x2C
#define KB_KEY_F            0x2D
#define KB_KEY_G            0x2E
#define KB_KEY_H            0x2F
#define KB_KEY_J            0x30
#define KB_KEY_K            0x31
#define KB_KEY_L            0x32
#define KB_KEY_COLON        0x33
#define KB_KEY_SEMICOLON    KB_KEY_COLON
#define KB_KEY_SINGLEQUOTE  0x34
#define KB_KEY_QUOTE        KB_KEY_SINGLEQUOTE
#define KB_KEY_ENTER        0x35

#define KB_KEY_LSHIFT       0x36
#define KB_KEY_LEFTSHIFT    KB_KEY_LSHIFT
#define KB_KEY_Z            0x37
#define KB_KEY_X            0x38
#define KB_KEY_C            0x39
#define KB_KEY_V            0x3A
#define KB_KEY_B            0x3B
#define KB_KEY_N            0x3C
#define KB_KEY_M            0x3D
#define KB_KEY_COMMA        0x3E
#define KB_KEY_LESS         KB_KEY_COMMA
#define KB_KEY_PERIOD       0x3F
#define KB_KEY_GREATER      KB_KEY_PERIOD
#define KB_KEY_SLASH        0x40
#define KB_KEY_QUESTION     KB_KEY_SLASH
#define KB_KEY_RSHIFT       0x41
#define KB_KEY_RIGHTSHIFT   KB_KEY_RSHIFT

#define KB_KEY_LCTRL        0x42
#define KB_KEY_LEFTCTRL     KB_KEY_LCTRL
#define KB_KEY_SUPER        0x43
#define KB_KEY_LALT         0x44
#define KB_KEY_LEFTALT      KB_KEY_LALT
#define KB_KEY_SPACE        0x45
#define KB_KEY_RALT         0x46
#define KB_KEY_RIGHTALT     KB_KEY_RALT
#define KB_KEY_FN           0x47
#define KB_KEY_RCTRL        0x48
#define KB_KEY_RIGHTCTRL    KB_KEY_RCTRL

#define KB_KEY_LEFT         0x49
#define KB_KEY_UP           0x4A
#define KB_KEY_DOWN         0x4B
#define KB_KEY_RIGHT        0x4C

char KEYCODE_TO_UNICODE[256] =
{
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, '`', '0', '1',
    '2', '3', '4', '5', '6', '7', '8', '9',
    '-', '=', '\b', '\t', 'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\\', 0x0, 'a', 's', 'd', 'f', 'g', 'h', 
    'j','k', 'l', ';', '\'', '\n', 0x0, 'z', 
    'x','c', 'v', 'b', 'n', 'm', ',', '.', 
    '/', 0x0, 0x0, 0x0, 0x0, ' ', 0x0, 0x0, 
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, '~', ')', '!',
    '@', '#', '$', '%', '^', '&', '*', '(',
    '_', '+', 0x0, 0x0, 'Q', 'W', 'E', 'R',
    'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
    '|', 0x0, 'A', 'S', 'D', 'F', 'G', 'H',
    'J', 'K', 'L', ':', '"', 0x0, 0x0, 'Z',
    'X', 'C', 'V', 'B', 'N', 'M', '<', '>',
    '?', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
};

#endif