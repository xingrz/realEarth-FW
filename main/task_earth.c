#include "task_earth.h"

static const char *TAG = "task_earth";

static uint8_t jpeg_buf[20 * 1024] = {0};
static uint16_t pixels_buf[SCREEN_SIZE * SCREEN_SIZE] = {0};

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

		esp_err_t ret = decode_image(jpeg_buf, pixels_buf);
		if (ret != ESP_OK) {
			ESP_LOGW(TAG, "Failed decoding JPEG");
			goto next;
		}

		lcd_draw(pixels_buf);

	next:
		vTaskDelay(10 * 60 * 1000 / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}
