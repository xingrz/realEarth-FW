#include "common.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "tasks.h"

#define TAG "main"

void
app_main(void)
{
	ESP_LOGI(TAG, "realEarth %s (%d, %s)", GIT_TAG, GIT_INCREMENT, GIT_COMMIT);

	ESP_ERROR_CHECK(esp_event_loop_create_default());

	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	assert(pdPASS == xTaskCreate(lv_proc_task, "lv_proc_task", LV_PROC_STACK_SIZE, NULL,
							 tskIDLE_PRIORITY + 1, NULL));

	assert(pdPASS == xTaskCreate(lcd_proc_task, "lcd_proc_task", LCD_PROC_STACK_SIZE, NULL,
							 tskIDLE_PRIORITY + 1, NULL));

	assert(pdPASS == xTaskCreate(btn_proc_task, "btn_proc_task", BTN_PROC_STACK_SIZE, NULL,
							 tskIDLE_PRIORITY + 1, NULL));

	assert(pdPASS == xTaskCreate(wlan_proc_task, "wlan_proc_task", WLAN_PROC_STACK_SIZE, NULL,
							 tskIDLE_PRIORITY + 1, NULL));

	assert(pdPASS == xTaskCreate(ntp_proc_task, "ntp_proc_task", NTP_PROC_STACK_SIZE, NULL,
							 tskIDLE_PRIORITY + 1, NULL));

	assert(pdPASS == xTaskCreate(earth_proc_task, "earth_proc_task", EARTH_PROC_STACK_SIZE, NULL,
							 tskIDLE_PRIORITY + 1, NULL));

	assert(pdPASS == xTaskCreate(ble_proc_task, "ble_proc_task", BLE_PROC_STACK_SIZE, NULL,
							 tskIDLE_PRIORITY + 1, NULL));

	ESP_LOGI(TAG, "SYSTEM READY");
}
