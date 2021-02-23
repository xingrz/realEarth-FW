#include "tasks.h"
#include "task_wlan.h"

#define TAG "task_wlan"

static xQueueHandle wlan_q = NULL;

typedef enum {
	e_wlan_set = 1,
	e_wlan_unset = 2,
} wlan_msg_e;

typedef struct {
	wlan_msg_e what;
	char ssid[WLAN_SSID_LEN];
	char password[WLAN_PASSWORD_LEN];
} wlan_msg_t;

static char wlan_ssid[WLAN_SSID_LEN] = {0};

void
wlan_proc_task(void *arg)
{
	wlan_q = xQueueCreate(3, sizeof(wlan_msg_t));

	ESP_LOGI(TAG, "Init Wi-Fi...");
	wlan_init();

	userdata_t user = {0};
	userdata_read(&user);
	if (strlen(user.ssid) > 0) {
		lcd_show_loading();
		ESP_LOGI(TAG, "Read stored Wi-Fi ssid: %s, password: %s", user.ssid, user.password);
		if (wlan_connect(user.ssid, user.password) == ESP_OK) {
			ESP_LOGI(TAG, "Wi-Fi connected");
			strcpy(wlan_ssid, user.ssid);
		} else {
			lcd_show_offline();
			ESP_LOGI(TAG, "Wi-Fi not connectable");
		}
	} else {
		lcd_show_offline();
		ESP_LOGI(TAG, "Wi-Fi not set");
	}

	wlan_msg_t msg = {0};
	while (1) {
		if (xQueueReceive(wlan_q, &msg, portMAX_DELAY) != pdPASS) {
			goto next;
		}

		switch (msg.what) {
			case e_wlan_set:
				ESP_LOGI(TAG, "Connecting to Wi-Fi \"%s\"...", msg.ssid);
				wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
				ESP_ERROR_CHECK(esp_wifi_init(&cfg));
				if (wlan_connect(msg.ssid, msg.password) == ESP_OK) {
					userdata_t user = {0};
					strcpy(user.ssid, msg.ssid);
					strcpy(user.password, msg.password);
					userdata_write(&user);
					ESP_LOGI(TAG, "Settings stored");
					vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);
					esp_restart();
				} else {
					lcd_show_qrcode();
					ESP_LOGI(TAG, "Setting up failed");
					memset(wlan_ssid, 0, sizeof(wlan_ssid));
				}
				break;
			case e_wlan_unset:
				ESP_LOGI(TAG, "Clearing Wi-Fi settings...");
				wlan_disconnect();
				userdata_clear();
				memset(wlan_ssid, 0, sizeof(wlan_ssid));
				break;
		}

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

	wlan_msg_t msg = {.what = e_wlan_set};
	strcpy(msg.ssid, ssid);
	strcpy(msg.password, password);
	xQueueSend(wlan_q, &msg, 0);
}

void
wlan_reset(void)
{
	wlan_msg_t msg = {.what = e_wlan_unset};
	xQueueSend(wlan_q, &msg, 0);
}

bool
wlan_configured(void)
{
	return strlen(wlan_ssid) > 0;
}
