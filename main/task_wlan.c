#include "task_wlan.h"

static const char *TAG = "task_wlan";

void
wlan_proc_task(void *arg)
{
	ESP_LOGI(TAG, "Init netif...");
	ESP_ERROR_CHECK(esp_netif_init());

	xEventGroupSetBits((EventGroupHandle_t)arg, BOOT_TASK_WLAN);

	ESP_LOGI(TAG, "Connect WLAN...");
	ESP_ERROR_CHECK(wlan_connect());

	while (1) {
		vTaskDelay(10);
	}

	vTaskDelete(NULL);
}
