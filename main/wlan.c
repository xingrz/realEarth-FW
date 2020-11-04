#include "wlan.h"

#include "lwip/err.h"
#include "lwip/sys.h"

static const char *TAG = "wlan";

static esp_netif_t *s_esp_netif = NULL;
static esp_ip4_addr_t s_ip_addr;
static xSemaphoreHandle s_semph_get_ip_addrs;

static void
on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
	ESP_ERROR_CHECK(esp_wifi_connect());
}

static void
on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
	memcpy(&s_ip_addr, &event->ip_info.ip, sizeof(s_ip_addr));
	xSemaphoreGive(s_semph_get_ip_addrs);
}

static void
start(char *ssid, char *password)
{
	esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
	s_esp_netif = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);

	esp_wifi_set_default_wifi_sta_handlers();

	ESP_ERROR_CHECK(esp_event_handler_register(
			WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL));

	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	wifi_config_t wifi_config = {.sta = {.ssid = {0}, .password = {0}}};
	strcpy((char *)wifi_config.sta.ssid, ssid);
	strcpy((char *)wifi_config.sta.password, password);
	ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_connect());

	s_semph_get_ip_addrs = xSemaphoreCreateBinary();
}

static void
stop(void)
{
	ESP_ERROR_CHECK(esp_event_handler_unregister(
			WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect));
	ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip));

	esp_err_t err = esp_wifi_stop();
	if (err == ESP_ERR_WIFI_NOT_INIT) {
		return;
	}
	ESP_ERROR_CHECK(err);
	ESP_ERROR_CHECK(esp_wifi_deinit());
	ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(s_esp_netif));
	esp_netif_destroy(s_esp_netif);
	s_esp_netif = NULL;
}

esp_err_t
wlan_connect(char *ssid, char *password)
{
	if (s_semph_get_ip_addrs != NULL) {
		return ESP_ERR_INVALID_STATE;
	}
	start(ssid, password);
	ESP_ERROR_CHECK(esp_register_shutdown_handler(&stop));
	ESP_LOGI(TAG, "Waiting for IP(s)");
	xSemaphoreTake(s_semph_get_ip_addrs, portMAX_DELAY);
	ESP_LOGI(TAG, "IPv4 address: " IPSTR, IP2STR(&s_ip_addr));
	return ESP_OK;
}

esp_err_t
wlan_disconnect(void)
{
	if (s_semph_get_ip_addrs == NULL) {
		return ESP_ERR_INVALID_STATE;
	}
	vSemaphoreDelete(s_semph_get_ip_addrs);
	s_semph_get_ip_addrs = NULL;
	stop();
	ESP_ERROR_CHECK(esp_unregister_shutdown_handler(&stop));
	return ESP_OK;
}
