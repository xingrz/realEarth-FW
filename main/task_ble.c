#include "task_ble.h"

static const char *TAG = "task_ble";

static void
ble_recv_cb(void *buf, uint16_t len)
{
	ESP_LOGI(TAG, "recv: %d", len);
}

void
ble_proc_task(void *arg)
{
	xEventGroupSetBits((EventGroupHandle_t)arg, BOOT_TASK_BLE);

	blec_init(ble_recv_cb);
	ESP_LOGI(TAG, "BLE init done");

	vTaskDelete(NULL);
}
