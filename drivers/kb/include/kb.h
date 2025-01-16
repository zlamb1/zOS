#ifndef KB_H
#define KB_H 1

#include <stdbool.h>
#include <stdint.h>

#define KB_BUF_SIZE 32

typedef struct
{
    uint8_t keycode;
    uint8_t pressed : 1;
    uint8_t shift   : 1; 
    uint8_t ctrl    : 1;
    uint8_t caps    : 1;
    uint8_t alt     : 1;
    uint8_t super   : 1;
} KeyInfo; 

extern uint32_t kb_buf_size, kb_buf_first, kb_buf_last;
extern uint8_t scancode_buffer[KB_BUF_SIZE];  
extern bool keycode_buffer[128]; 

bool has_key_info(void); 
KeyInfo dequeue_key_info(void); 
char get_ascii_from_key_info(KeyInfo info);

#endif