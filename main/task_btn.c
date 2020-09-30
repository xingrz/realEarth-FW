#include "task_btn.h"

static const char *TAG = "task_btn";

#define PIN_KEY_USER GPIO_NUM_0

#define BL_MIN 2
#define BL_MAX GC9A01_BACKLIGHT_MAX
#define BL_STEP BL_MAX / 5

static xQueueHandle btn_q = NULL;

static int16_t bl_lvl = BL_MAX;
static int16_t bl_step = -BL_STEP;

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
	uint32_t lvl;
	while (1) {
		if (xQueueReceive(btn_q, &num, portMAX_DELAY)) {
			val = gpio_get_level(num);
			ESP_LOGV(TAG, "GPIO: %d=%d", num, val);
			if (!val) {
				bl_lvl += bl_step;
				if (bl_lvl <= BL_MIN) {
					bl_step = -bl_step;
					lvl = BL_MIN;
				} else if (bl_lvl >= BL_MAX) {
					bl_step = -bl_step;
					lvl = BL_MAX;
				} else {
					lvl = bl_lvl;
				}
				gc9a01_backlight(lvl);
			}
		}
	}

	vTaskDelete(NULL);
}
