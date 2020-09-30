#include "hspi.h"

static const char *TAG = "hspi";

#define DMA_CHAN 2

static spi_device_handle_t spi = {0};

void
hspi_init(void)
{
	ESP_LOGV(TAG, "hspi_init enter");

	spi_bus_config_t bus = {
			.mosi_io_num = PIN_MOSI,
			.sclk_io_num = PIN_CLK,
			.quadwp_io_num = -1,
			.quadhd_io_num = -1,
			.max_transfer_sz = HSPI_MAX_LEN,
	};

	ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &bus, DMA_CHAN));

	spi_device_interface_config_t dev = {
			.clock_speed_hz = 26 * 1000 * 1000,
			.mode = 0,
			.spics_io_num = PIN_CS,
			.queue_size = 7,
	};

	ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &dev, &spi));

	ESP_LOGV(TAG, "hspi_init exit");
}

void
hspi_write(void *buf, uint32_t len)
{
	spi_transaction_t trans = {
			.tx_buffer = buf,
			.length = len * 8,
	};

	ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &trans));
}
