#include "task_lcd.h"

static const char *TAG = "task_lcd";

#define WHITE 0xFFFF
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0

void
lcd_proc_task(void *arg)
{
	ESP_LOGI(TAG, "Init LCD...");
	gc9a01_init();
	gc9a01_fill(0x0000);

	ESP_LOGI(TAG, "Enable backlight...");
	gc9a01_set_backlight(GC9A01_BACKLIGHT_MAX);

	xEventGroupSetBits((EventGroupHandle_t)arg, BOOT_TASK_LCD);

	while (1) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		gc9a01_fill(MAGENTA);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		gc9a01_fill(GREEN);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		gc9a01_fill(CYAN);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		gc9a01_fill(YELLOW);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		gc9a01_fill(WHITE);
	}

	vTaskDelete(NULL);
}
