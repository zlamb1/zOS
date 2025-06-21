#include "vga.h"
#include "machine_io.h"
#include "text.h"

#define MISC_OUTPUT_READ_REG     0x3CC
#define MISC_OUTPUT_WRITE_REG    0x3C2
#define CRTC_ADDRESS_REG         0x3D4
#define CRTC_DATA_REG            0x3D5
#define CRTC_MAX_SCANLINE_REG    0x009
#define CRTC_CURSOR_START_REG    0x00A
#define CRTC_CURSOR_END_REG      0x00B
#define CRTC_CURSOR_HIGH_LOC_REG 0x00E
#define CRTC_CURSOR_LOW_LOC_REG  0x00F

static void vgaputchar (root_term_t *t, char c);
static void vgaputchar_unsynced (root_term_t *root_term_t, char c);
static root_text_pos_t vgagetpos (root_term_t *t);
static void vgaadvance (root_term_t *t, root_u16 x, root_u16 y);
static void vgasetpos (root_term_t *t, root_u16 x, root_u16 y);
static void vgasetcursorpos (root_term_t *t, root_u16 x, root_u16 y);
static void vgasetcursorvisible (root_term_t *t, root_u8 visible);
static void vgaclear (root_term_t *t, root_u32 color);

int
root_initvga (root_vga_term_t *out)
{
  root_u8 fg = ROOT_VGA_COLOR_GREEN;
  root_u8 bg = ROOT_VGA_COLOR_BLACK;
  root_vga_cursor_t vgacursor
      = { .base = { .x = 0, .y = 0, .visible = 0 }, .color = bg << 4 | fg };

  out->pos.x = 0;
  out->pos.y = 0;

  out->base.type = ROOT_TERM_TYPE_VGA;
  out->base.attribs.tabsize = 4;
  out->base.attribs.scrolltype = ROOT_SCROLL_TYPE_NEWLINE;

  out->base.putchar = vgaputchar;
  out->base.putchar_unsynced = vgaputchar_unsynced;
  out->base.getpos = vgagetpos;
  out->base.advance = vgaadvance;
  out->base.setpos = vgasetpos;
  out->base.setcursorpos = vgasetcursorpos;
  out->base.setcursorvisible = vgasetcursorvisible;
  out->base.clear = vgaclear;

  out->cursor = vgacursor;

  out->state.width = 80;
  out->state.height = 25;
  out->state.stride = out->state.width << 1;
  out->state.data = (root_u8 *) 0xB8000;

  root_outb (CRTC_ADDRESS_REG, CRTC_MAX_SCANLINE_REG);
  out->state.max_scanline = root_inb (CRTC_DATA_REG) & 0x1F;

  vgaclear (&out->base, bg << 4 | fg);
  vgasetcursorpos (&out->base, vgacursor.base.x, vgacursor.base.y);
  vgasetcursorvisible (&out->base, 1);

  return 0;
}

void
vgaputchar (root_term_t *t, char c)
{
  root_vga_term_t *vga = (root_vga_term_t *) t;
  vgaputchar_unsynced (t, c);
  vgasetcursorpos (t, vga->pos.x, vga->pos.y);
}

static void
vgaputchar_unsynced (root_term_t *t, char c)
{
  root_vga_term_t *vga = (root_vga_term_t *) t;
  root_u8 *data = vga->state.data;
  data += vga->pos.y * vga->state.stride + (vga->pos.x << 1);
  switch (c)
    {
    case '\t':
      for (root_u32 i = 0; i < vga->base.attribs.tabsize; i++)
        vgaputchar_unsynced (t, ' ');
      break;
    case '\r':
      vgaadvance (t, 0, 1);
      break;
    case '\n':
      vgaadvance (t, ROOT_TEXT_POS_RESET, 1);
      break;
    default:
      data[0] = (root_u8) c;
      data[1] = vga->cursor.color;
      vgaadvance (t, 1, 0);
      break;
    }
}

root_text_pos_t
vgagetpos (root_term_t *t)
{
  root_vga_term_t *vga = (root_vga_term_t *) t;
  return vga->pos;
}

void
vgaadvance (root_term_t *t, root_u16 x, root_u16 y)
{
  root_vga_term_t *vga = (root_vga_term_t *) t;
  root_u16 nx = vga->pos.x;
  root_u16 ny = vga->pos.y;

  if (x != ROOT_TEXT_POS_RESET)
    {
      nx += x;
      ny += nx / vga->state.width;
      nx %= vga->state.width;
    }
  else
    {
      nx = 0;
    }
  if (y != ROOT_TEXT_POS_RESET)
    {
      ny += y;
      switch (vga->base.attribs.scrolltype)
        {
        case ROOT_SCROLL_TYPE_WRAP:
          ny %= vga->state.height;
          break;
        case ROOT_SCROLL_TYPE_NEWLINE:
          if (ny >= vga->state.height)
            {
              root_u16 rows = ny - vga->state.height + 1,
                       clearstart = vga->state.height - rows;
              root_u8 *src = vga->state.data + rows * vga->state.stride,
                      *dst = vga->state.data;
              for (root_u16 ry = 0; ry < clearstart; ry++)
                {
                  for (root_u16 rx = 0; rx < vga->state.width; rx++)
                    {
                      root_u16 idx = rx << 1;
                      dst[idx] = src[idx];
                      dst[idx + 1] = src[idx + 1];
                    }
                  src += vga->state.stride;
                  dst += vga->state.stride;
                }
              dst = vga->state.data + clearstart * vga->state.stride;
              for (root_u16 ry = clearstart; ry < vga->state.height; ry++)
                {
                  for (root_u16 rx = 0; rx < vga->state.width; rx++, dst += 2)
                    {
                      dst[0] = 0;
                      dst[1] = vga->cursor.color;
                    }
                }
              ny = vga->state.height - 1;
            }
          break;
        }
    }
  else if (y == ROOT_TEXT_POS_RESET)
    ny = 0;

  vga->pos.x = nx;
  vga->pos.y = ny;
}

void
vgasetpos (root_term_t *t, root_u16 x, root_u16 y)
{
  root_vga_term_t *vga = (root_vga_term_t *) t;
  // TODO: validate X/Y
  vga->pos.x = x;
  vga->pos.y = y;
}

void
vgasetcursorpos (root_term_t *t, root_u16 x, root_u16 y)
{
  root_vga_term_t *vga = (root_vga_term_t *) t;
  root_u16 pos = y * vga->state.width + x;
  vga->cursor.base.x = x;
  vga->cursor.base.y = y;
  root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_LOW_LOC_REG);
  root_outb (CRTC_DATA_REG, pos & 0xFF);
  root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_HIGH_LOC_REG);
  root_outb (CRTC_DATA_REG, (pos >> 8) & 0xFF);
}

void
vgasetcursorvisible (root_term_t *t, root_u8 visible)
{
  root_vga_term_t *vga = (root_vga_term_t *) t;
  if (vga->cursor.base.visible != visible)
    {
      if (visible == 1)
        {
          root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_START_REG);
          root_outb (CRTC_DATA_REG, (root_inb (CRTC_DATA_REG) & 0xC0));
          root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_END_REG);
          root_outb (CRTC_DATA_REG, (root_inb (CRTC_DATA_REG) & 0xE0)
                                        | vga->state.max_scanline);
        }
      else
        {
          root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_START_REG);
          root_outb (CRTC_DATA_REG, 0x20);
        }
      vga->cursor.base.visible = visible;
    }
}

void
vgaclear (root_term_t *t, root_u32 color)
{
  root_vga_term_t *vga = (root_vga_term_t *) t;
  root_u8 *data = vga->state.data;
  for (root_u16 y = 0; y < vga->state.height; y++)
    {
      for (root_u16 x = 0; x < vga->state.width; x++, data += 2)
        {
          data[0] = 0;
          data[1] = color;
        }
    }
}
