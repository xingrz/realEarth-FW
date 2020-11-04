#include "task_wlan.h"

static const char *TAG = "task_wlan";

static xQueueHandle wlan_q = NULL;

typedef struct {
	char ssid[WLAN_SSID_LEN];
	char password[WLAN_PASSWORD_LEN];
} wlan_msg_t;

static char wlan_ssid[WLAN_SSID_LEN];

void
wlan_proc_task(void *arg)
{
	wlan_q = xQueueCreate(3, sizeof(wlan_msg_t));

	ESP_LOGI(TAG, "Init netif...");
	ESP_ERROR_CHECK(esp_netif_init());

	xEventGroupSetBits((EventGroupHandle_t)arg, BOOT_TASK_WLAN);

	userdata_t user = {0};
	userdata_read(&user);
	if (strlen(user.ssid) > 0) {
		ESP_LOGI(TAG, "Read stored Wi-Fi ssid: %s, password: %s", user.ssid, user.password);
		wlan_connect(user.ssid, user.password);
	} else {
		ESP_LOGI(TAG, "Wi-Fi not set");
	}

	wlan_msg_t msg = {0};
	while (1) {
		if (xQueueReceive(wlan_q, &msg, portMAX_DELAY) != pdPASS) {
			goto next;
		}

		ESP_LOGI(TAG, "Connecting to WLAN \"%s\"...", msg.ssid);
		wlan_connect(msg.ssid, msg.password);

		userdata_t user = {0};
		strcpy(user.ssid, msg.ssid);
		strcpy(user.password, msg.password);
		userdata_write(&user);
		ESP_LOGI(TAG, "Settings stored");

		vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

		esp_restart();

	next:
		vTaskDelay(10);
	}

	vTaskDelete(NULL);
}

void
wlan_setup(char *ssid, char *password)
{
	if (strcmp(wlan_ssid, ssid) == 0) {
		ESP_LOGI(TAG, "Setting up %s, ignored", ssid);
		return;
	}
	strcpy(wlan_ssid, ssid);

	wlan_msg_t msg = {0};
	strcpy(msg.ssid, ssid);
	strcpy(msg.password, password);
	xQueueSend(wlan_q, &msg, 0);
}
