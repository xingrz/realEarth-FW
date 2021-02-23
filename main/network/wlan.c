#include "common.h"
#include "nvs.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "wlan.h"

static const char *TAG = "wlan";

#define MAX_ALLOW_FAILS 5

static esp_ip4_addr_t s_ip_addr;
static bool s_connecting = false;
static bool s_disconnecting = false;
static uint8_t s_fails = 0;
static xSemaphoreHandle s_semph_result;

static void
on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_data;
	ESP_LOGW(TAG, "Wi-Fi disconnected for reason: %d", event->reason);
	if (s_disconnecting) {
		return;
	}
	s_fails++;
	if (s_fails < MAX_ALLOW_FAILS) {
		ESP_ERROR_CHECK(esp_wifi_connect());
	} else if (s_connecting) {
		xSemaphoreGive(s_semph_result);
	}
}

static void
on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
	memcpy(&s_ip_addr, &event->ip_info.ip, sizeof(s_ip_addr));
	s_fails = 0;
	s_connecting = false;
	xSemaphoreGive(s_semph_result);
}

static void
start(void)
{
	s_fails = 0;
	s_connecting = true;

	ESP_ERROR_CHECK(esp_event_handler_register(
			WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_connect());
}

static void
stop(void)
{
	s_disconnecting = true;

	ESP_ERROR_CHECK(esp_event_handler_unregister(
			WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect));
	ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip));

	esp_wifi_stop();
}

esp_err_t
wlan_init(void)
{
	ESP_ERROR_CHECK(esp_netif_init());

	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));

	return ESP_OK;
}

esp_err_t
wlan_connect()
{
	if (s_semph_result != NULL) {
		return ESP_ERR_INVALID_STATE;
	}

	s_semph_result = xSemaphoreCreateBinary();

	start();

	ESP_LOGI(TAG, "Waiting for IP(s)");
	xSemaphoreTake(s_semph_result, portMAX_DELAY);
	vSemaphoreDelete(s_semph_result);
	s_semph_result = NULL;

	if (s_fails == 0) {
		ESP_LOGI(TAG, "IPv4 address: " IPSTR, IP2STR(&s_ip_addr));
		return ESP_OK;
	} else {
		ESP_LOGW(TAG, "Failed connecting");
		stop();
		return ESP_ERR_WIFI_NOT_CONNECT;
	}
}

esp_err_t
wlan_set(const char *ssid, const char *password)
{
	ESP_LOGI(TAG, "Connecting to %s...", ssid);
	wifi_config_t config = {0};
	strcpy((char *)config.sta.ssid, ssid);
	strcpy((char *)config.sta.password, password);
	return esp_wifi_set_config(ESP_IF_WIFI_STA, &config);
}

esp_err_t
wlan_reset(void)
{
	if (s_semph_result != NULL) {
		vSemaphoreDelete(s_semph_result);
		s_semph_result = NULL;
	}
	stop();

	wifi_config_t config = {0};
	return esp_wifi_set_config(ESP_IF_WIFI_STA, &config);
}

bool
wlan_configured(char *ssid)
{
	wifi_config_t config = {0};
	esp_wifi_get_config(ESP_IF_WIFI_STA, &config);
	if (strlen((const char *)config.sta.ssid) > 0) {
		if (ssid != NULL) {
			strcpy(ssid, (const char *)config.sta.ssid);
		}
		return true;
	} else {
		return false;
	}
}
