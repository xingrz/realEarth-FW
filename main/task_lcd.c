#include "task_lcd.h"

static const char *TAG = "task_lcd";

static xQueueHandle lcd_q = NULL;

void
lcd_proc_task(void *arg)
{
	lcd_q = xQueueCreate(10, sizeof(uint16_t *));

	ESP_LOGI(TAG, "Init LCD...");
	gc9a01_init();
	gc9a01_fill(0x0000);

	ESP_LOGI(TAG, "Enable backlight...");
	gc9a01_backlight(GC9A01_BACKLIGHT_MAX);

	xEventGroupSetBits((EventGroupHandle_t)arg, BOOT_TASK_LCD);

	uint16_t *pixels = NULL;
	while (1) {
		if (xQueueReceive(lcd_q, &pixels, portMAX_DELAY) != pdPASS) {
			vTaskDelay(10);
			continue;
		}

		gc9a01_draw(pixels);
	}

	vTaskDelete(NULL);
}

void
lcd_draw(uint16_t *pixels)
{
	xQueueSendToBack(lcd_q, &pixels, 0);
}
