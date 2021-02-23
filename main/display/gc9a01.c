#include "common.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"

#include "pinout.h"
#include "gc9a01.h"

#define TAG "gc9a01"

#define DMA_CHAN 2

#define TRANS_CMD (void *)0
#define TRANS_DATA (void *)1

#define GC9A01_FLAP 1

typedef struct {
	uint16_t start;
	uint16_t end;
} gc9a01_write_addresses_t;

static spi_device_handle_t spi;

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
#if GC9A01_FLAP
		{0x36, 1, {0x08 | 0x40}},
#else
		{0x36, 1, {0x08}},
#endif
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

static void IRAM_ATTR
hspi_write(void *buf, uint32_t len, void *dc)
{
	spi_transaction_t trans = {
			.tx_buffer = buf,
			.length = len * 8,
			.user = dc,
	};

	ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &trans));
}

static void IRAM_ATTR
write_reg(uint8_t val)
{
	hspi_write(&val, sizeof(uint8_t), TRANS_CMD);
}

static void IRAM_ATTR
write_data(void *buf, uint32_t len)
{
	hspi_write(buf, len, TRANS_DATA);
}

static void IRAM_ATTR
spi_pre_transfer_callback(spi_transaction_t *t)
{
	gpio_set_level(PIN_LCD_DC, (uint32_t)t->user);
}

void
gc9a01_init(void)
{
	ESP_LOGI(TAG, "Init interfaces...");

	gpio_config_t output = {
			.intr_type = GPIO_INTR_DISABLE,
			.pin_bit_mask = (1UL << PIN_LCD_DC) | (1UL << PIN_LCD_RST),
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_ENABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
	};

	ESP_ERROR_CHECK(gpio_config(&output));

	spi_bus_config_t bus = {
			.mosi_io_num = PIN_LCD_MOSI,
			.miso_io_num = -1,
			.sclk_io_num = PIN_LCD_CLK,
			.quadwp_io_num = -1,
			.quadhd_io_num = -1,
			.max_transfer_sz = GC9A01_BUF_SIZE * sizeof(uint16_t),
			.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_IOMUX_PINS,
	};

	ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &bus, DMA_CHAN));

	spi_device_interface_config_t dev = {
			.clock_speed_hz = SPI_MASTER_FREQ_16M,
			.mode = 0,
			.spics_io_num = PIN_LCD_CS,
			.queue_size = 7,
			.pre_cb = spi_pre_transfer_callback,
	};

	ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &dev, &spi));

	ESP_LOGI(TAG, "Reset panel...");

	gpio_set_level(PIN_LCD_RST, 0);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	gpio_set_level(PIN_LCD_RST, 1);

	ESP_LOGI(TAG, "Init GC9A01...");

	for (int i = 0; i < COUNT(gc9a01_reg_conf); i++) {
		write_reg(gc9a01_reg_conf[i].cmd);
		if (gc9a01_reg_conf[i].len > 0) {
			write_data(gc9a01_reg_conf[i].data, gc9a01_reg_conf[i].len);
		}
	}
	vTaskDelay(100 / portTICK_PERIOD_MS);
	write_reg(0x11);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	write_reg(0x29);
	vTaskDelay(100 / portTICK_PERIOD_MS);
}

#define SWAP(u16) (((u16 & 0xFF) << 8) | (u16 >> 8))

static void
gc9a01_prepare(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2)
{
	gc9a01_write_addresses_t addrs;

	write_reg(0x2A);
	addrs.start = SWAP(x1);
	addrs.end = SWAP(x2);
	write_data(&addrs, sizeof(addrs));

	write_reg(0x2B);
	addrs.start = SWAP(y1);
	addrs.end = SWAP(y2);
	write_data(&addrs, sizeof(addrs));
}

#if GC9A01_INIT_BLANK
void
gc9a01_fill(uint16_t color)
{
	gc9a01_prepare(0, (uint8_t)(GC9A01_WIDTH - 1), 0, (uint8_t)(GC9A01_HEIGHT - 1));

	uint16_t pt[GC9A01_WIDTH * 8];
	memset(pt, color, sizeof(pt));

	write_reg(0x2C);
	for (int y = 0; y < GC9A01_HEIGHT; y += 8) {
		write_data(pt, sizeof(pt));
	}
}
#endif

void
gc9a01_draw(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint16_t *src)
{
	gc9a01_prepare(x1, x2, y1, y2);

	write_reg(0x2C);
	write_data(src, (x2 - x1 + 1) * (y2 - y1 + 1) * sizeof(uint16_t));
}
