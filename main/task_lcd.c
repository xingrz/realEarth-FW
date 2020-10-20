#include "task_lcd.h"

static const char *TAG = "task_lcd";

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
		vTaskDelay(10);
	}

	vTaskDelete(NULL);
}
