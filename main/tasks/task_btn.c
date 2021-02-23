#include "common.h"

#include "freertos/queue.h"
#include "driver/gpio.h"

#include "tasks.h"
#include "task_lcd.h"
#include "gc9a01.h"
#include "wlan.h"
#include "blec.h"

#define TAG "task_btn"

#define PIN_KEY_USER GPIO_NUM_0
#define LONG_PRESS_TIMEOUT_MS 1000

static xQueueHandle btn_q = NULL;
static TickType_t btn_down_ticks = 0;

static void
drv_gpio_event(void *arg)
{
	uint32_t num = (uint32_t)arg;
	xQueueSendFromISR(btn_q, &num, NULL);
}

static void
drv_gpio_init()
{
	gpio_config_t input = {
			.intr_type = GPIO_INTR_ANYEDGE,
			.pin_bit_mask = (1UL << PIN_KEY_USER),
			.mode = GPIO_MODE_INPUT,
			.pull_up_en = GPIO_PULLUP_ENABLE,
	};

	gpio_config(&input);

	gpio_install_isr_service(0);
	gpio_isr_handler_add(PIN_KEY_USER, drv_gpio_event, (void *)PIN_KEY_USER);
}

static void
btn_on_pressed(void)
{
	ESP_LOGI(TAG, "Pressed");
}

static void
btn_on_long_pressed(void)
{
	ESP_LOGI(TAG, "Long pressed");
	wlan_reset();
	lcd_show_qrcode();
	gc9a01_set_backlight(GC9A01_BACKLIGHT_MAX);
	blec_adv_start();
}

static void
btn_on_down(void)
{
	ESP_LOGV(TAG, "Down");
}

static void
btn_on_up(void)
{
	ESP_LOGV(TAG, "Up");
}

void
btn_proc_task(void *arg)
{
	btn_q = xQueueCreate(3, sizeof(uint32_t));

	drv_gpio_init();

	uint32_t num;
	int val;
	while (1) {
		if (btn_down_ticks > 0) {
			val = gpio_get_level(num);
			if (val) {
				btn_down_ticks = 0;
				btn_on_pressed();
			} else {
				TickType_t ticks = xTaskGetTickCount();
				uint32_t hold_ms = (ticks - btn_down_ticks) * portTICK_PERIOD_MS;
				if (hold_ms >= LONG_PRESS_TIMEOUT_MS) {
					btn_down_ticks = 0;
					btn_on_long_pressed();
				} else {
					goto next;
				}
			}
		}

		if (xQueueReceive(btn_q, &num, portMAX_DELAY) != pdPASS) {
			goto next;
		}

		val = gpio_get_level(num);
		if (!val) {
			btn_down_ticks = xTaskGetTickCount();
			btn_on_down();
		} else {
			btn_down_ticks = 0;
			btn_on_up();
		}

	next:
		vTaskDelay(10);
	}

	vTaskDelete(NULL);
}
