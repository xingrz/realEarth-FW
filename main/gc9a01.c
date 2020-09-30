#include "gc9a01.h"

static const char *TAG = "gc9a01";

#define HSPI_MAX_PIXELS (HSPI_MAX_LEN / sizeof(uint16_t))

static void
drv_gpio_init()
{
	ESP_LOGV(TAG, "drv_gpio_init enter");

	gpio_config_t output = {
			.intr_type = GPIO_INTR_DISABLE,
			.pin_bit_mask = (1UL << PIN_BLK) | (1UL << PIN_DC) | (1UL << PIN_RST),
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_ENABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
	};

	gpio_config(&output);

	ESP_LOGV(TAG, "drv_gpio_init exit");
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
	hspi_init();

	ESP_LOGI(TAG, "Reset panel...");
	gpio_set_level(PIN_RST, 0);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	gpio_set_level(PIN_RST, 1);

	ESP_LOGI(TAG, "Init GC9A01...");
	write_reg(0xEF);
	write_reg(0xEB);
	write_data8(0x14);

	write_reg(0xFE);
	write_reg(0xEF);
	write_reg(0xEB);
	write_data8(0x14);

	write_reg(0x84);
	write_data8(0x40);

	write_reg(0x85);
	write_data8(0xFF);

	write_reg(0x86);
	write_data8(0xFF);

	write_reg(0x87);
	write_data8(0xFF);

	write_reg(0x88);
	write_data8(0x0A);

	write_reg(0x89);
	write_data8(0x21);

	write_reg(0x8A);
	write_data8(0x00);

	write_reg(0x8B);
	write_data8(0x80);

	write_reg(0x8C);
	write_data8(0x01);

	write_reg(0x8D);
	write_data8(0x01);

	write_reg(0x8E);
	write_data8(0xFF);

	write_reg(0x8F);
	write_data8(0xFF);

	write_reg(0xB6);
	write_data8(0x00);
	write_data8(0x20);

	write_reg(0x36);
	write_data8(0x08);

	write_reg(0x3A);
	write_data8(0x05);

	write_reg(0x90);
	write_data8(0x08);
	write_data8(0x08);
	write_data8(0x08);
	write_data8(0x08);

	write_reg(0xBD);
	write_data8(0x06);

	write_reg(0xBC);
	write_data8(0x00);

	write_reg(0xFF);
	write_data8(0x60);
	write_data8(0x01);
	write_data8(0x04);

	write_reg(0xC3);
	write_data8(0x13);
	write_reg(0xC4);
	write_data8(0x13);

	write_reg(0xC9);
	write_data8(0x22);

	write_reg(0xBE);
	write_data8(0x11);

	write_reg(0xE1);
	write_data8(0x10);
	write_data8(0x0E);

	write_reg(0xDF);
	write_data8(0x21);
	write_data8(0x0c);
	write_data8(0x02);

	write_reg(0xF0);
	write_data8(0x45);
	write_data8(0x09);
	write_data8(0x08);
	write_data8(0x08);
	write_data8(0x26);
	write_data8(0x2A);

	write_reg(0xF1);
	write_data8(0x43);
	write_data8(0x70);
	write_data8(0x72);
	write_data8(0x36);
	write_data8(0x37);
	write_data8(0x6F);

	write_reg(0xF2);
	write_data8(0x45);
	write_data8(0x09);
	write_data8(0x08);
	write_data8(0x08);
	write_data8(0x26);
	write_data8(0x2A);

	write_reg(0xF3);
	write_data8(0x43);
	write_data8(0x70);
	write_data8(0x72);
	write_data8(0x36);
	write_data8(0x37);
	write_data8(0x6F);

	write_reg(0xED);
	write_data8(0x1B);
	write_data8(0x0B);

	write_reg(0xAE);
	write_data8(0x77);

	write_reg(0xCD);
	write_data8(0x63);

	write_reg(0x70);
	write_data8(0x07);
	write_data8(0x07);
	write_data8(0x04);
	write_data8(0x0E);
	write_data8(0x0F);
	write_data8(0x09);
	write_data8(0x07);
	write_data8(0x08);
	write_data8(0x03);

	write_reg(0xE8);
	write_data8(0x34);

	write_reg(0x62);
	write_data8(0x18);
	write_data8(0x0D);
	write_data8(0x71);
	write_data8(0xED);
	write_data8(0x70);
	write_data8(0x70);
	write_data8(0x18);
	write_data8(0x0F);
	write_data8(0x71);
	write_data8(0xEF);
	write_data8(0x70);
	write_data8(0x70);

	write_reg(0x63);
	write_data8(0x18);
	write_data8(0x11);
	write_data8(0x71);
	write_data8(0xF1);
	write_data8(0x70);
	write_data8(0x70);
	write_data8(0x18);
	write_data8(0x13);
	write_data8(0x71);
	write_data8(0xF3);
	write_data8(0x70);
	write_data8(0x70);

	write_reg(0x64);
	write_data8(0x28);
	write_data8(0x29);
	write_data8(0xF1);
	write_data8(0x01);
	write_data8(0xF1);
	write_data8(0x00);
	write_data8(0x07);

	write_reg(0x66);
	write_data8(0x3C);
	write_data8(0x00);
	write_data8(0xCD);
	write_data8(0x67);
	write_data8(0x45);
	write_data8(0x45);
	write_data8(0x10);
	write_data8(0x00);
	write_data8(0x00);
	write_data8(0x00);

	write_reg(0x67);
	write_data8(0x00);
	write_data8(0x3C);
	write_data8(0x00);
	write_data8(0x00);
	write_data8(0x00);
	write_data8(0x01);
	write_data8(0x54);
	write_data8(0x10);
	write_data8(0x32);
	write_data8(0x98);

	write_reg(0x74);
	write_data8(0x10);
	write_data8(0x85);
	write_data8(0x80);
	write_data8(0x00);
	write_data8(0x00);
	write_data8(0x4E);
	write_data8(0x00);

	write_reg(0x98);
	write_data8(0x3e);
	write_data8(0x07);

	write_reg(0x35);
	write_reg(0x21);

	vTaskDelay(120 / portTICK_PERIOD_MS);

	write_reg(0x11);

	vTaskDelay(120 / portTICK_PERIOD_MS);

	write_reg(0x29);

	vTaskDelay(120 / portTICK_PERIOD_MS);
}

void
gc9a01_backlight(uint8_t level)
{
	ESP_LOGI(TAG, "Set backlight: %d", level);
	gpio_set_level(PIN_BLK, !!level);
}

void
gc9a01_fill(uint16_t color)
{
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
