#include "task_earth.h"

static const char *TAG = "task_earth";

#define JPEG_BUF_SIZE (20 * 1024)
#define PIXELS_BUF_CNT (SCREEN_SIZE * SCREEN_SIZE)

void
earth_proc_task(void *arg)
{
	time_t now;
	struct tm timeinfo;

	xEventGroupSetBits((EventGroupHandle_t)arg, BOOT_TASK_EARTH);

	uint8_t *jpeg_buf = (uint8_t *)malloc(JPEG_BUF_SIZE);
	uint16_t *pixels_buf = (uint16_t *)malloc(PIXELS_BUF_CNT * sizeof(uint16_t));

	while (1) {
		time(&now);
		localtime_r(&now, &timeinfo);
		if (timeinfo.tm_year < (2016 - 1900)) {
			ESP_LOGI(TAG, "Time is not set, skipped");
			vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
			continue;
		}

		uint32_t fetched = earth_fetch(now, jpeg_buf, JPEG_BUF_SIZE);
		if (fetched == 0) {
			goto next;
		}

		ESP_LOGI(TAG, "Fetched %d bytes", fetched);

		esp_err_t ret = decode_image(jpeg_buf, pixels_buf);
		if (ret != ESP_OK) {
			ESP_LOGW(TAG, "Failed decoding JPEG");
			goto next;
		}

		lcd_draw(pixels_buf);

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

	free(jpeg_buf);
	free(pixels_buf);

	vTaskDelete(NULL);
}
