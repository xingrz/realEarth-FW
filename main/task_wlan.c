#include "task_wlan.h"

static const char *TAG = "task_wlan";

static xQueueHandle wlan_q = NULL;

typedef struct {
	char ssid[33];
	char password[64];
} wlan_msg_t;

void
wlan_proc_task(void *arg)
{
	wlan_q = xQueueCreate(3, sizeof(wlan_msg_t));

	ESP_LOGI(TAG, "Init netif...");
	ESP_ERROR_CHECK(esp_netif_init());

	xEventGroupSetBits((EventGroupHandle_t)arg, BOOT_TASK_WLAN);

	wlan_msg_t msg = {0};
	while (1) {
		if (xQueueReceive(wlan_q, &msg, portMAX_DELAY) != pdPASS) {
			goto next;
		}

		ESP_LOGI(TAG, "Connecting to WLAN \"%s\"...", msg.ssid);
		wlan_connect(msg.ssid, msg.password);

	next:
		vTaskDelay(10);
	}

	vTaskDelete(NULL);
}

void
wlan_setup(char *ssid, char *password)
{
	wlan_msg_t msg = {0};
	strcpy(msg.ssid, ssid);
	strcpy(msg.password, password);
	xQueueSend(wlan_q, &msg, portMAX_DELAY);
}
