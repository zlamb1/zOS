#ifndef KB_H
#define KB_H 1

#include <stdbool.h>
#include <stdint.h>

#include "types.h"

#define KB_BUF_SIZE 32

struct key_event
{
    u8 keycode;
    u8 pressed : 1;
    u8 shift   : 1; 
    u8 ctrl    : 1;
    u8 caps    : 1;
    u8 alt     : 1;
    u8 super   : 1;
}; 

extern u32 kb_buf_size, kb_buf_first, kb_buf_last;
extern u8 scancode_buffer[KB_BUF_SIZE];  
extern bool keycode_buffer[128]; 

bool has_key_event(void); 
struct key_event dequeue_key_event(void); 
char get_unicode_from_key_info(struct key_event event);

#endif