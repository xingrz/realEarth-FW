#ifndef __REALEARTH_GC9A01__
#define __REALEARTH_GC9A01__

#include <stdint.h>

#define SCREEN_SIZE 240

void gc9a01_init(void);
void gc9a01_fill(uint16_t color);
void gc9a01_draw(uint16_t *pixels);
void gc9a01_draw_part_start(void);
void gc9a01_draw_part_lines(uint16_t *pixels, uint16_t lines);

#endif  // __REALEARTH_GC9A01__
