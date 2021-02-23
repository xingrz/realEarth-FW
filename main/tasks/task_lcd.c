#include "common.h"
#include "freertos/queue.h"

#include "tasks.h"
#include "task_lcd.h"
#include "gc9a01.h"
#include "decode_image.h"

#define TAG "task_lcd"

#define DRAW_RECT_SIZE 16

static xQueueHandle lcd_q = NULL;
static uint16_t pixels_buf[SCREEN_SIZE * DRAW_RECT_SIZE] = {0};

static uint8_t *lcd_bg = NULL;

extern const uint8_t pic_loading_start[] asm("_binary_pic_loading_jpg_start");
extern const uint8_t pic_offline_start[] asm("_binary_pic_offline_jpg_start");
extern const uint8_t pic_qrcode_start[] asm("_binary_pic_qrcode_jpg_start");

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
lcd_show_offline(void)
{
	lcd_draw_bg((uint8_t *)pic_offline_start);
}

void
lcd_show_loading(void)
{
	lcd_draw_bg((uint8_t *)pic_loading_start);
}

void
lcd_show_qrcode(void)
{
	lcd_draw_bg((uint8_t *)pic_qrcode_start);
}

void
lcd_draw_bg(uint8_t *jpeg)
{
	lcd_bg = jpeg;
	xQueueSendToBack(lcd_q, &jpeg, portMAX_DELAY);
}

void
lcd_draw_fg(uint8_t *jpeg)
{
	xQueueSendToBack(lcd_q, &jpeg, portMAX_DELAY);
}

void
lcd_clear_fg(void)
{
	if (lcd_bg != NULL) {
		xQueueSendToBack(lcd_q, &lcd_bg, portMAX_DELAY);
	}
}
