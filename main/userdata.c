#include "userdata.h"

static const char *TAG = "userdata";

#define TYPE_CUSTOM 0x40
#define SUBTYPE_USERDATA 0x00

static const esp_partition_t *part = NULL;

esp_err_t
userdata_init(void)
{
	ESP_LOGI(TAG, "Init");

	part = esp_partition_find_first(TYPE_CUSTOM, SUBTYPE_USERDATA, NULL);
	if (part == NULL) {
		ESP_LOGE(TAG, "Partition not found!");
		return ESP_ERR_INVALID_STATE;
	}

	ESP_LOGI(TAG, "Partition found. address: 0x%08x, size: %d, label: %s", part->address,
			part->size, part->label);

	return ESP_OK;
}

esp_err_t
userdata_read(userdata_t *data)
{
	esp_err_t ret;

	uint8_t *buf = (uint8_t *)data;
	ret = esp_flash_read(part->flash_chip, buf, part->address, sizeof(userdata_t));
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed reading partition");
		return ret;
	}

	uint64_t crc = 0;
	for (int i = 0; i < sizeof(userdata_t) - 1; i++) {
		crc += buf[i];
	}
	if (buf[sizeof(userdata_t) - 1] != (crc & 0xff)) {
		ESP_LOGI(TAG, "Invalid CRC, ignored read data");
		memset(data, 0, sizeof(userdata_t));
		return ESP_OK;
	}

	return ESP_OK;
}

esp_err_t
userdata_write(userdata_t *data)
{
	esp_err_t ret;

	ret = esp_flash_erase_region(part->flash_chip, part->address, part->size);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed erasing partition for writing");
		return ret;
	}

	uint8_t *buf = (uint8_t *)data;
	uint64_t crc = 0;
	for (int i = 0; i < sizeof(userdata_t) - 1; i++) {
		crc += buf[i];
	}
	data->crc = (crc & 0xff);

	return esp_flash_write(part->flash_chip, data, part->address, sizeof(userdata_t));
}
