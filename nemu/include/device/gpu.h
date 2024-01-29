#ifndef __GPU_H__
#define __GPU_H__

#include "nemu.h"
#include "device/port_io.h"
#include "cpu/reg.h"
#include "device/port_io.h"
#include "memory/mmu/page.h"
#include "memory/mmu/segment.h"
#include "memory.h"
#include "nemu.h"
#include <stdint.h>

#define GPU_DATA_PORT_BASE 0x5da
#define GPU_CMD_PORT 0x5de

#define BLIT_CMD 0x01

void gpu_init();
make_pio_handler(gpu_data_handler);
make_pio_handler(gpu_cmd_handler);

typedef struct GPU_CMD{
  vaddr_t src_surface,dst_surface,src_rect,dst_rect;
} GPU_CMD;

typedef struct
{
	int16_t x, y;
	uint16_t w, h;
} SDL_Rect;

typedef struct
{
	uint8_t r, g, b, unused;
} SDL_Color;

typedef struct
{
	int ncolors;
	SDL_Color *colors;
} SDL_Palette;

typedef struct
{
	SDL_Palette *palette;
	uint8_t BitsPerPixel;
	uint8_t BytesPerPixel;
	uint8_t Rloss, Gloss, Bloss, Aloss;
	uint8_t Rshift, Gshift, Bshift, Ashift;
	uint32_t Rmask, Gmask, Bmask, Amask;
	uint32_t colorkey;
	uint8_t alpha;
} SDL_PixelFormat;

typedef struct
{
	uint32_t flags;
	SDL_PixelFormat *format;
	int w, h;
	uint16_t pitch;
	SDL_Rect clip_rect;

	int refcount;
	uint8_t *pixels;

} SDL_Surface;

#endif
