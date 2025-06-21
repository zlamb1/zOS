#include "bios.h"
#ifndef ZVBE_H
#define ZVBE_H 1

#include "types.h"

typedef struct root_vbe_info_t
{
  char sig[4];
  root_u16 ver;
  const char *oem;
  root_u32 capabilities;
  root_u16 modes_offset;
  root_u16 modes_segment;
  root_u16 video_memory;
  root_u16 oem_ver;
  root_u32 vendor_name;
  root_u32 product_name;
  root_u32 product_revision;
  root_u16 vbe_ver;
  root_u32 accelerated_modes;
  char reserved[472];
} __attribute__ ((packed)) root_vbe_info_t;

typedef struct root_vbe_mode_info_t
{
  root_u16 attributes;
  root_u8 window_a;
  root_u8 window_b;
  root_u16 granularity;
  root_u16 window_size;
  root_u16 segment_a;
  root_u16 segment_b;
  root_u32 win_func_ptr;
  root_u16 pitch;
  root_u16 width;
  root_u16 height;
  root_u8 w_char;
  root_u8 y_char;
  root_u8 planes;
  root_u8 bpp;
  root_u8 banks;
  root_u8 memory_model;
  root_u8 bank_size;
  root_u8 image_pages;
  root_u8 reserved0;
  root_u8 red_mask;
  root_u8 red_pos;
  root_u8 green_mask;
  root_u8 green_pos;
  root_u8 blue_mask;
  root_u8 blue_pos;
  root_u8 reserved_mask;
  root_u8 reserved_pos;
  root_u8 color_attributes;
  root_u8 *framebuffer;
  root_u32 off_screen_mem_off;
  root_u32 off_screen_mem_size;
  root_u8 reserved1[206];
} __attribute__ ((packed)) root_vbe_mode_info_t;

static void root_queryvbeinfo (void);
static int root_queryvbemodeinfo (root_vbe_mode_info_t *mode_info,
                                  root_u16 mode);

void
root_queryvbeinfo (void)
{
  root_bios_args_t args;
  root_vbe_info_t info;
  root_vbe_mode_info_t mode_info;
  root_u16 *modes;
  root_uintptr_t address = (root_uintptr_t) &info;
  args.eax = 0x4F00;
  args.es = root_get_segment (address);
  args.edi = root_get_offset (address);
  root_bios_interrupt (0x10, &args);
  modes
      = (root_u16 *) root_get_pointer (info.modes_segment, info.modes_offset);
  (void) modes;
  (void) mode_info;
  (void) root_queryvbemodeinfo;
}

int
root_queryvbemodeinfo (root_vbe_mode_info_t *mode_info, root_u16 mode)
{
  root_bios_args_t args;
  root_uintptr_t address = (root_uintptr_t) mode_info;
  args.eax = 0x4F01;
  args.ecx = mode;
  args.es = root_get_segment (address);
  args.edi = root_get_offset (address);
  root_bios_interrupt (0x10, &args);
  (void) root_queryvbeinfo;
  return (mode_info->attributes & 0x90) == 0x90
         && (mode_info->memory_model == 4 || mode_info->memory_model == 6)
         && mode_info->bpp == 32 && mode_info->width == 1920
         && mode_info->height == 1080;
}

#endif
