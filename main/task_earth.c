#include "task_earth.h"

static const char *TAG = "task_earth";

#define DRAW_RECT_SIZE 16

static uint8_t jpeg_buf[20 * 1024] = {0};
static uint16_t pixels_buf[SCREEN_SIZE * DRAW_RECT_SIZE] = {0};

static inline uint16_t
rgb888_to_rgb565(uint8_t *in)
{
	uint16_t v = 0;
	v |= ((in[0] >> 3) << 11);
	v |= ((in[1] >> 2) << 5);
	v |= ((in[2] >> 3) << 0);
	return v;
}

static void
jpeg_decode_cb(uint8_t *in, uint16_t left, uint16_t right, uint16_t top, uint16_t bottom)
{
	for (int y = top; y <= bottom; y++) {
		for (int x = left; x <= right; x++) {
			uint16_t v = rgb888_to_rgb565(in);
			v = (v >> 8) | (v << 8);
			pixels_buf[(y % DRAW_RECT_SIZE) * SCREEN_SIZE + x] = v;
			in += 3;
		}
	}

	if (right + 1 == SCREEN_SIZE) {
		gc9a01_draw_part_lines(pixels_buf, DRAW_RECT_SIZE);
	}
}

void
earth_proc_task(void *arg)
{
	time_t now;
	struct tm timeinfo;

	xEventGroupSetBits((EventGroupHandle_t)arg, BOOT_TASK_EARTH);

	while (1) {
		time(&now);
		localtime_r(&now, &timeinfo);
		if (timeinfo.tm_year < (2016 - 1900)) {
			ESP_LOGI(TAG, "Time is not set, skipped");
			vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
			continue;
		}

		uint32_t fetched = earth_fetch(now, jpeg_buf, sizeof(jpeg_buf));
		if (fetched == 0) {
			goto next;
		}

		ESP_LOGI(TAG, "Fetched %d bytes", fetched);

		gc9a01_draw_part_start();
		esp_err_t ret = decode_image(jpeg_buf, jpeg_decode_cb);
		if (ret != ESP_OK) {
			ESP_LOGW(TAG, "Failed decoding JPEG");
			goto next;
		}

		if (timeinfo.tm_hour >= 20) {
			gc9a01_set_backlight(0);
		} else if (timeinfo.tm_hour >= 18) {
			gc9a01_set_backlight(1);
		} else if (timeinfo.tm_hour >= 16) {
			gc9a01_set_backlight(2);
		} else if (timeinfo.tm_hour >= 14) {
			gc9a01_set_backlight(3);
		} else if (timeinfo.tm_hour >= 10) {
			gc9a01_set_backlight(4);
		} else if (timeinfo.tm_hour >= 8) {
			gc9a01_set_backlight(3);
		} else if (timeinfo.tm_hour >= 6) {
			gc9a01_set_backlight(2);
		} else if (timeinfo.tm_hour >= 4) {
			gc9a01_set_backlight(1);
		} else {
			gc9a01_set_backlight(0);
		}

	next:
		vTaskDelay(10 * 60 * 1000 / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}
