#ifndef _CLOCK_GC9A01_
#define _CLOCK_GC9A01_

#include "common.h"

#define PIN_BLK GPIO_NUM_10
#define PIN_CS GPIO_NUM_15
#define PIN_DC GPIO_NUM_5
#define PIN_RST GPIO_NUM_4
#define PIN_MOSI GPIO_NUM_13
#define PIN_CLK GPIO_NUM_14

#define SCREEN_SIZE 240

void gc9a01_init(void);
void gc9a01_backlight(uint8_t level);
void gc9a01_fill(uint16_t color);

#endif  // _CLOCK_GC9A01_
