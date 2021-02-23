#include "common.h"

#define TAG "main"

void
app_main(void)
{
	ESP_LOGI(TAG, "realEarth %s (%d, %s)", GIT_TAG, GIT_INCREMENT, GIT_COMMIT);

	ESP_ERROR_CHECK(esp_event_loop_create_default());

	ESP_ERROR_CHECK(userdata_init());

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
