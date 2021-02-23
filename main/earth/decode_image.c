/*
The image used for the effect on the LCD in the SPI master example is stored in flash
as a jpeg file. This file contains the decode_image routine, which uses the tiny JPEG
decoder library to decode this JPEG into a format that can be sent to the display.

Keep in mind that the decoder library cannot handle progressive files (will give
``Image decoder: jd_prepare failed (8)`` as an error) so make sure to save in the correct
format if you want to use a different image file.
*/

#include "common.h"
#include "decode_image.h"
#include "tjpgd.h"

const char *TAG = "decode_image";

// Size of the work space for the jpeg decoder.
#define WORKSZ 3100

typedef struct {
	const uint8_t *input;
	uint16_t input_pos;
	decode_cb output_cb;
} decode_ctx_t;

static uint16_t
infunc(JDEC *decoder, uint8_t *buf, uint16_t len)
{
	decode_ctx_t *ctx = (decode_ctx_t *)decoder->device;
	if (buf != NULL) {
		memcpy(buf, ctx->input + ctx->input_pos, len);
	}
	ctx->input_pos += len;
	return len;
}

static uint16_t
outfunc(JDEC *decoder, void *bitmap, JRECT *rect)
{
	decode_ctx_t *ctx = (decode_ctx_t *)decoder->device;
	ctx->output_cb((uint8_t *)bitmap, rect->left, rect->right, rect->top, rect->bottom);
	return 1;
}

esp_err_t
decode_image(uint8_t *input, decode_cb cb)
{
	esp_err_t ret = ESP_OK;
	uint8_t work[WORKSZ] = {0};
	int r;
	JDEC decoder;

	decode_ctx_t ctx = {
			.input = input,
			.input_pos = 0,
			.output_cb = cb,
	};

	// Prepare and decode the jpeg.
	r = jd_prepare(&decoder, infunc, &work, WORKSZ, (void *)&ctx);
	if (r != JDR_OK) {
		ESP_LOGE(TAG, "Image decoder: jd_prepare failed (%d)", r);
		ret = ESP_ERR_NOT_SUPPORTED;
		goto exit;
	}

	r = jd_decomp(&decoder, outfunc, 0);
	if (r != JDR_OK && r != JDR_FMT1) {
		ESP_LOGE(TAG, "Image decoder: jd_decode failed (%d)", r);
		ret = ESP_ERR_NOT_SUPPORTED;
		goto exit;
	}

exit:
	return ret;
}
