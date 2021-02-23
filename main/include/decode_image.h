#ifndef __REALEARTH_DECODE_IMAGE__
#define __REALEARTH_DECODE_IMAGE__

#include <stdint.h>
#include "esp_err.h"

typedef void (*decode_cb)(
		uint8_t *in, uint16_t left, uint16_t right, uint16_t top, uint16_t bottom);

esp_err_t decode_image(uint8_t *input, decode_cb cb);

#endif  // __REALEARTH_DECODE_IMAGE__
