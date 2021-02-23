#include "wlan.h"

#include "lwip/err.h"
#include "lwip/sys.h"

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
start(char *ssid, char *password)
{
	s_fails = 0;
	s_connecting = true;

	ESP_ERROR_CHECK(esp_event_handler_register(
			WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL));

	wifi_config_t wifi_config = {.sta = {.ssid = {0}, .password = {0}}};
	strcpy((char *)wifi_config.sta.ssid, ssid);
	strcpy((char *)wifi_config.sta.password, password);
	ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
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
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	ESP_ERROR_CHECK(esp_netif_init());

	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	return ESP_OK;
}

esp_err_t
wlan_connect(char *ssid, char *password)
{
	if (s_semph_result != NULL) {
		return ESP_ERR_INVALID_STATE;
	}

	s_semph_result = xSemaphoreCreateBinary();

	start(ssid, password);

	ESP_LOGI(TAG, "Waiting for IP(s)");
	xSemaphoreTake(s_semph_result, portMAX_DELAY);
	vSemaphoreDelete(s_semph_result);
	s_semph_result = NULL;

	if (s_fails == 0) {
		ESP_LOGI(TAG, "IPv4 address: " IPSTR, IP2STR(&s_ip_addr));
		return ESP_OK;
	} else {
		ESP_LOGW(TAG, "Failed connecting to %s", ssid);
		stop();
		return ESP_ERR_WIFI_NOT_CONNECT;
	}
}

esp_err_t
wlan_disconnect(void)
{
	if (s_semph_result != NULL) {
		vSemaphoreDelete(s_semph_result);
		s_semph_result = NULL;
	}
	stop();
	return ESP_OK;
}
