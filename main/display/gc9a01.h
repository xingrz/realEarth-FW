#ifndef __REALEARTH_GC9A01__
#define __REALEARTH_GC9A01__

#include <stdint.h>

#define GC9A01_WIDTH 256
#define GC9A01_HEIGHT 240

#define GC9A01_BUF_WIDTH GC9A01_WIDTH
#define GC9A01_BUF_HEIGHT 64
#define GC9A01_BUF_SIZE (GC9A01_BUF_WIDTH * GC9A01_BUF_HEIGHT)

#define GC9A01_INIT_BLANK 0

void gc9a01_init(void);
#if GC9A01_INIT_BLANK
void gc9a01_fill(uint16_t color);
#endif
void gc9a01_draw(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint16_t *src);

#endif  // __REALEARTH_GC9A01__
