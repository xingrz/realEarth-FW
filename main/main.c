#include "common.h"

static const char *TAG = "main";

void
app_main(void)
{
	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	BaseType_t ret = pdFALSE;
	EventGroupHandle_t boot = xEventGroupCreate();

	ret = xTaskCreate(lcd_proc_task, "lcd_proc_task", 2048, boot, 10, NULL);
	if (ret != pdPASS) ESP_LOGE(TAG, "Failed xTaskCreate(lcd_proc_task): %d", ret);

	ret = xTaskCreate(btn_proc_task, "btn_proc_task", 2048, boot, 10, NULL);
	if (ret != pdPASS) ESP_LOGE(TAG, "Failed xTaskCreate(btn_proc_task): %d", ret);

	ret = xTaskCreate(wlan_proc_task, "wlan_proc_task", 4096, boot, 10, NULL);
	if (ret != pdPASS) ESP_LOGE(TAG, "Failed xTaskCreate(wlan_proc_task): %d", ret);

	ret = xTaskCreate(ntp_proc_task, "ntp_proc_task", 2048, boot, 10, NULL);
	if (ret != pdPASS) ESP_LOGE(TAG, "Failed xTaskCreate(ntp_proc_task): %d", ret);

	ret = xTaskCreate(earth_proc_task, "earth_proc_task", 20 * 1024, boot, 10, NULL);
	if (ret != pdPASS) ESP_LOGE(TAG, "Failed xTaskCreate(earth_proc_task): %d", ret);

	xEventGroupWaitBits(boot, BOOT_TASK_ALL, pdFALSE, pdTRUE, portMAX_DELAY);
	ESP_LOGI(TAG, "SYSTEM READY");

	while (1) {
		vTaskDelay(10);
	}
}
