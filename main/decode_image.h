#ifndef _CLOCK_DECODE_IMAGE_
#define _CLOCK_DECODE_IMAGE_

#include "common.h"

typedef void (*decode_cb)(
		uint8_t *in, uint16_t left, uint16_t right, uint16_t top, uint16_t bottom);

esp_err_t decode_image(uint8_t *input, decode_cb cb);

#endif  // _CLOCK_DECODE_IMAGE_
