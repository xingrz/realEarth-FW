#include "task_btn.h"

static const char *TAG = "task_btn";

#define PIN_KEY_USER GPIO_NUM_0

static xQueueHandle btn_q = NULL;

static uint16_t bl_step = -1;

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
			.intr_type = GPIO_INTR_NEGEDGE,
			.pin_bit_mask = (1UL << PIN_KEY_USER),
			.mode = GPIO_MODE_INPUT,
			.pull_up_en = GPIO_PULLUP_ENABLE,
	};

	gpio_config(&input);

	gpio_install_isr_service(0);
	gpio_isr_handler_add(PIN_KEY_USER, drv_gpio_event, (void *)PIN_KEY_USER);
}

void
btn_proc_task(void *arg)
{
	btn_q = xQueueCreate(10, sizeof(uint32_t));

	drv_gpio_init();

	xEventGroupSetBits((EventGroupHandle_t)arg, BOOT_TASK_BTN);

	uint32_t num;
	int val;
	while (1) {
		if (xQueueReceive(btn_q, &num, portMAX_DELAY)) {
			val = gpio_get_level(num);
			ESP_LOGV(TAG, "GPIO: %d=%d", num, val);
			if (!val) {
				uint16_t bl_lvl = gc9a01_get_backlight();
				if (bl_lvl >= GC9A01_BACKLIGHT_MAX) {
					bl_step = -1;
				} else if (bl_lvl <= 0) {
					bl_step = 1;
				}
				bl_lvl += bl_step;
				gc9a01_set_backlight(bl_lvl);
			}
		}
	}

	vTaskDelete(NULL);
}
