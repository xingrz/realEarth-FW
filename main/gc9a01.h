#ifndef _CLOCK_GC9A01_
#define _CLOCK_GC9A01_

#include "common.h"

#define PIN_BLK GPIO_NUM_4
#define PIN_CS GPIO_NUM_15
#define PIN_DC GPIO_NUM_2
#define PIN_RST GPIO_NUM_12
#define PIN_MOSI GPIO_NUM_13
#define PIN_CLK GPIO_NUM_14

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

#endif  // _CLOCK_GC9A01_
