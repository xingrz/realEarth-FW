#ifndef __REALEARTH_GC9A01__
#define __REALEARTH_GC9A01__

#include <stdint.h>

#define BLK_LEDC_TIMER LEDC_TIMER_0
#define BLK_LEDC_MODE LEDC_HIGH_SPEED_MODE
#define BLK_LEDC_CHANNEL LEDC_CHANNEL_0
#define BLK_LEDC_RES LEDC_TIMER_7_BIT

#define GC9A01_BACKLIGHT_MAX 4

#define SCREEN_SIZE 240

void gc9a01_init(void);
uint16_t gc9a01_get_backlight(void);
void gc9a01_set_backlight(uint16_t level);
void gc9a01_fill(uint16_t color);
void gc9a01_draw(uint16_t *pixels);
void gc9a01_draw_part_start(void);
void gc9a01_draw_part_lines(uint16_t *pixels, uint16_t lines);

#endif  // __REALEARTH_GC9A01__
