#include "wlan.h"

#include "tcpip_adapter.h"
#include "lwip/err.h"
#include "lwip/sys.h"

static const char *TAG = "wlan";

#define GOT_IPV4_BIT BIT(0)
#define CONNECTED_BITS (GOT_IPV4_BIT)

static EventGroupHandle_t s_connect_event_group;
static ip4_addr_t s_ip_addr;
static char s_connection_name[32] = CONFIG_PROJECT_WIFI_SSID;
static char s_connection_passwd[32] = CONFIG_PROJECT_WIFI_PASSWORD;

static void
on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	system_event_sta_disconnected_t *event = (system_event_sta_disconnected_t *)event_data;

	ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
	if (event->reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
		/*Switch to 802.11 bgn mode */
		esp_wifi_set_protocol(
				ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
	}
	ESP_ERROR_CHECK(esp_wifi_connect());
}

static void
on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
	memcpy(&s_ip_addr, &event->ip_info.ip, sizeof(s_ip_addr));
	xEventGroupSetBits(s_connect_event_group, GOT_IPV4_BIT);
}

static void
start(void)
{
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_ERROR_CHECK(esp_event_handler_register(
			WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL));

	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	wifi_config_t wifi_config = {0};

	strncpy((char *)&wifi_config.sta.ssid, s_connection_name, 32);
	strncpy((char *)&wifi_config.sta.password, s_connection_passwd, 32);

	ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_connect());
}

static void
stop(void)
{
	esp_err_t err = esp_wifi_stop();
	if (err == ESP_ERR_WIFI_NOT_INIT) {
		return;
	}
	ESP_ERROR_CHECK(err);

	ESP_ERROR_CHECK(esp_event_handler_unregister(
			WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect));
	ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip));

	ESP_ERROR_CHECK(esp_wifi_deinit());
}

esp_err_t
wlan_connect(void)
{
	if (s_connect_event_group != NULL) {
		return ESP_ERR_INVALID_STATE;
	}

	s_connect_event_group = xEventGroupCreate();
	start();
	xEventGroupWaitBits(s_connect_event_group, CONNECTED_BITS, true, true, portMAX_DELAY);
	ESP_LOGI(TAG, "Connected to %s", s_connection_name);
	ESP_LOGI(TAG, "IPv4 address: " IPSTR, IP2STR(&s_ip_addr));
	return ESP_OK;
}

esp_err_t
wlan_disconnect(void)
{
	if (s_connect_event_group == NULL) {
		return ESP_ERR_INVALID_STATE;
	}
	vEventGroupDelete(s_connect_event_group);
	s_connect_event_group = NULL;
	stop();
	ESP_LOGI(TAG, "Disconnected from %s", s_connection_name);
	s_connection_name[0] = '\0';
	return ESP_OK;
}
