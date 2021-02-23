#ifndef __REALEARTH_BACKLIGHT__
#define __REALEARTH_BACKLIGHT__

#include <stdint.h>

#define BACKLIGHT_MAX 4

void backlight_init(void);
uint8_t backlight_get(void);
void backlight_set(uint8_t level);

#endif  // __REALEARTH_BACKLIGHT__
