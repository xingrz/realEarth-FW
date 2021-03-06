#include <time.h>
#include <sys/time.h>

#include "common.h"

#include "tasks.h"
#include "task_lcd.h"
#include "wlan.h"
#include "earth.h"
#include "backlight.h"

#define TAG "task_earth"

// #define DEBUG_FORCE_NOON

static uint8_t jpeg_buf[20 * 1024] = {0};

void
earth_proc_task(void *arg)
{
	time_t now;
	struct tm timeinfo;

	while (1) {
		if (!wlan_configured(NULL)) {
			vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
			continue;
		}

		time(&now);
		localtime_r(&now, &timeinfo);
		if (timeinfo.tm_year < (2016 - 1900)) {
			ESP_LOGI(TAG, "Time is not set, skipped");
			vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
			continue;
		}

#ifdef DEBUG_FORCE_NOON
		if (timeinfo.tm_hour > 13) {
			now -= (timeinfo.tm_hour - 12) * 60 * 60;
		} else if (timeinfo.tm_hour < 11) {
			now -= (timeinfo.tm_hour + 12) * 60 * 60;
		}
		localtime_r(&now, &timeinfo);
#endif

		uint32_t fetched = earth_fetch(now, jpeg_buf, sizeof(jpeg_buf));
		if (fetched == 0) {
			goto next;
		}

		ESP_LOGI(TAG, "Fetched %d bytes", fetched);
		lcd_draw_bg(jpeg_buf);

		if (timeinfo.tm_hour >= 20) {
			backlight_set(0);
		} else if (timeinfo.tm_hour >= 18) {
			backlight_set(1);
		} else if (timeinfo.tm_hour >= 16) {
			backlight_set(2);
		} else if (timeinfo.tm_hour >= 14) {
			backlight_set(3);
		} else if (timeinfo.tm_hour >= 10) {
			backlight_set(4);
		} else if (timeinfo.tm_hour >= 8) {
			backlight_set(3);
		} else if (timeinfo.tm_hour >= 6) {
			backlight_set(2);
		} else if (timeinfo.tm_hour >= 4) {
			backlight_set(1);
		} else {
			backlight_set(0);
		}

	next:
		vTaskDelay(10 * 60 * 1000 / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}
