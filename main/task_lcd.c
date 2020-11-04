#include "task_lcd.h"

static const char *TAG = "task_lcd";

#define DRAW_RECT_SIZE 16

static xQueueHandle lcd_q = NULL;
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
lcd_proc_task(void *arg)
{
	lcd_q = xQueueCreate(3, sizeof(uint8_t *));

	ESP_LOGI(TAG, "Init LCD...");
	gc9a01_init();
	gc9a01_fill(0x0000);

	ESP_LOGI(TAG, "Enable backlight...");
	gc9a01_set_backlight(GC9A01_BACKLIGHT_MAX);

	xEventGroupSetBits((EventGroupHandle_t)arg, BOOT_TASK_LCD);

	uint8_t *jpeg = NULL;
	while (1) {
		if (xQueueReceive(lcd_q, &jpeg, portMAX_DELAY) != pdPASS) {
			goto next;
		}

		gc9a01_draw_part_start();
		esp_err_t ret = decode_image(jpeg, jpeg_decode_cb);
		if (ret != ESP_OK) {
			ESP_LOGW(TAG, "Failed decoding JPEG");
			goto next;
		}

	next:
		vTaskDelay(10);
	}

	vTaskDelete(NULL);
}

void
lcd_draw(uint8_t *jpeg)
{
	xQueueSendToBack(lcd_q, &jpeg, 0);
}
