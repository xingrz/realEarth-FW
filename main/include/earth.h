#ifndef __REALEARTH_EARTH__
#define __REALEARTH_EARTH__

#include <stdint.h>

uint32_t earth_fetch(time_t when, uint8_t *buf, uint32_t limit);

#endif  // __REALEARTH_EARTH__
