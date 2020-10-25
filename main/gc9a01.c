#include "gc9a01.h"

static const char *TAG = "gc9a01";

#define HSPI_MAX_PIXELS (HSPI_MAX_LEN / sizeof(uint16_t))

#define BLK_FADE_TIME 500

#define COUNT(x) (sizeof(x) / sizeof(x[0]))

static struct {
	uint8_t cmd;
	uint8_t len;
	uint8_t data[16];
} gc9a01_reg_conf[] = {
		{0xEF, 0, {}},
		{0xEB, 1, {0x14}},
		{0xFE, 0, {}},
		{0xEF, 0, {}},
		{0xEB, 1, {0x14}},
		{0x84, 1, {0x40}},
		{0x85, 1, {0xFF}},
		{0x86, 1, {0xFF}},
		{0x87, 1, {0xFF}},
		{0x88, 1, {0x0A}},
		{0x89, 1, {0x21}},
		{0x8A, 1, {0x00}},
		{0x8B, 1, {0x80}},
		{0x8C, 1, {0x01}},
		{0x8D, 1, {0x01}},
		{0x8E, 1, {0xFF}},
		{0x8F, 1, {0xFF}},
		{0xB6, 2, {0x00, 0x20}},
		{0x36, 1, {0x08}},
		{0x3A, 1, {0x05}},
		{0x90, 4, {0x08, 0x08, 0x08, 0x08}},
		{0xBD, 1, {0x06}},
		{0xBC, 1, {0x00}},
		{0xFF, 3, {0x60, 0x01, 0x04}},
		{0xC3, 1, {0x13}},
		{0xC4, 1, {0x13}},
		{0xC9, 1, {0x22}},
		{0xBE, 1, {0x11}},
		{0xE1, 2, {0x10, 0x0E}},
		{0xDF, 3, {0x21, 0x0C, 0x02}},
		{0xF0, 6, {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A}},
		{0xF1, 6, {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F}},
		{0xF2, 6, {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A}},
		{0xF3, 6, {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F}},
		{0xED, 2, {0x1B, 0x0B}},
		{0xAE, 1, {0x77}},
		{0xCD, 1, {0x63}},
		{0x70, 9, {0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03}},
		{0xE8, 1, {0x34}},
		{0x62, 12, {0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70}},
		{0x63, 12, {0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70}},
		{0x64, 7, {0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07}},
		{0x66, 10, {0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00}},
		{0x67, 10, {0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98}},
		{0x74, 7, {0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00}},
		{0x98, 2, {0x3e, 0x07}},
		{0x35, 0, {}},
		{0x21, 0, {}},
};

static void
drv_gpio_init()
{
	ESP_LOGV(TAG, "drv_gpio_init enter");

	gpio_config_t output = {
			.intr_type = GPIO_INTR_DISABLE,
			.pin_bit_mask = (1UL << PIN_DC) | (1UL << PIN_RST),
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_ENABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
	};

	gpio_config(&output);

	ESP_LOGV(TAG, "drv_gpio_init exit");
}

static void
drv_pwm_init()
{
	ESP_LOGV(TAG, "drv_pwm_init enter");

	ledc_timer_config_t timer = {
			.duty_resolution = BLK_LEDC_RES,
			.freq_hz = 5000,
			.speed_mode = BLK_LEDC_MODE,
			.timer_num = BLK_LEDC_TIMER,
			.clk_cfg = LEDC_AUTO_CLK,
	};

	ledc_timer_config(&timer);

	ledc_channel_config_t channel = {
			.channel = BLK_LEDC_CHANNEL,
			.duty = 0,
			.gpio_num = PIN_BLK,
			.speed_mode = BLK_LEDC_MODE,
			.hpoint = 0,
			.timer_sel = BLK_LEDC_TIMER,
	};

	ledc_channel_config(&channel);

	ledc_fade_func_install(0);

	ESP_LOGV(TAG, "drv_pwm_init exit");
}

static void
write_reg(uint8_t val)
{
	gpio_set_level(PIN_DC, 0);
	hspi_write(&val, sizeof(uint8_t));
}

static void
write_data(void *buf, uint32_t len)
{
	gpio_set_level(PIN_DC, 1);
	hspi_write(buf, len);
}

static void
write_data8(uint8_t val)
{
	gpio_set_level(PIN_DC, 1);
	hspi_write(&val, sizeof(uint8_t));
}

static void
write_data16(uint16_t val)
{
	gpio_set_level(PIN_DC, 1);
	hspi_write(&val, sizeof(uint16_t));
}

void
gc9a01_init(void)
{
	ESP_LOGI(TAG, "Init interfaces...");
	drv_gpio_init();
	drv_pwm_init();
	hspi_init();

	ESP_LOGI(TAG, "Reset panel...");
	gpio_set_level(PIN_RST, 0);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	gpio_set_level(PIN_RST, 1);

	ESP_LOGI(TAG, "Init GC9A01...");

	for (int i = 0; i < COUNT(gc9a01_reg_conf); i++) {
		write_reg(gc9a01_reg_conf[i].cmd);
		if (gc9a01_reg_conf[i].len > 0) {
			write_data(gc9a01_reg_conf[i].data, gc9a01_reg_conf[i].len);
		}
	}

	vTaskDelay(120 / portTICK_PERIOD_MS);

	write_reg(0x11);

	vTaskDelay(120 / portTICK_PERIOD_MS);

	write_reg(0x29);

	vTaskDelay(120 / portTICK_PERIOD_MS);
}

void
gc9a01_backlight(uint16_t level)
{
	ESP_LOGI(TAG, "Set backlight: %d", level);
	ledc_set_fade_with_time(BLK_LEDC_MODE, BLK_LEDC_CHANNEL, level, BLK_FADE_TIME);
	ledc_fade_start(BLK_LEDC_MODE, BLK_LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
}

void
gc9a01_fill(uint16_t color)
{
	ESP_LOGI(TAG, "Fill color %04X", color);

	write_reg(0x2A);
	write_data16(0);
	write_data16(SCREEN_SIZE - 1);

	write_reg(0x2B);
	write_data16(0);
	write_data16(SCREEN_SIZE - 1);

	uint16_t pt[HSPI_MAX_PIXELS];
	for (int i = 0; i < HSPI_MAX_PIXELS; i++) {
		pt[i] = color;
	}

	write_reg(0x2C);
	for (int y = 0; y < SCREEN_SIZE; y++) {
		// The number of regulators each line is 256 + 1
		for (int x = 0; x < 256 / HSPI_MAX_PIXELS; x++) {
			write_data(pt, HSPI_MAX_PIXELS * sizeof(uint16_t));
		}
		write_data(pt, 1 * sizeof(uint16_t));
	}
}
