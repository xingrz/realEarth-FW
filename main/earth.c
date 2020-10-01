#include "earth.h"

static const char *TAG = "earth";

typedef struct {
	uint8_t *buf;
	uint32_t limit;
	uint32_t read;
} earth_http_ctx_t;

static esp_err_t
earth_http_event(esp_http_client_event_t *evt)
{
	switch (evt->event_id) {
		case HTTP_EVENT_ERROR:
			ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
			break;
		case HTTP_EVENT_ON_DATA: {
			earth_http_ctx_t *ctx = (earth_http_ctx_t *)evt->user_data;
			uint32_t len = evt->data_len;
			if (ctx->read + len > ctx->limit) {
				len = ctx->limit - ctx->read;
			}
			memcpy(ctx->buf + ctx->read, evt->data, len);
			ctx->read += len;
			break;
		}
		default:
			break;
	}
	return ESP_OK;
}

uint32_t
earth_fetch(time_t when, uint8_t *buf, uint32_t limit)
{
	uint32_t ret = 0;
	char url[128];

	struct tm timeinfo;
	when -= 8 * 60 * 60;
	when -= 40 * 60;
	localtime_r(&when, &timeinfo);

	sprintf(url, CONFIG_PROJECT_EARTH_URL, (1900 + timeinfo.tm_year), (timeinfo.tm_mon + 1),
			timeinfo.tm_mday, timeinfo.tm_hour, (timeinfo.tm_min / 10 * 10));
	ESP_LOGI(TAG, "Requesting %s...", url);

	earth_http_ctx_t ctx = {
			.buf = buf,
			.limit = limit,
			.read = 0,
	};

	esp_http_client_config_t config = {
			.url = url,
			.event_handler = earth_http_event,
			.user_data = (void *)&ctx,
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_err_t err = esp_http_client_perform(client);
	if (err != ESP_OK) {
		goto exit;
	}

	int status_code = esp_http_client_get_status_code(client);
	int content_length = esp_http_client_get_content_length(client);
	ESP_LOGI(TAG, "status: %d, content_length: %d", status_code, content_length);

	if (content_length != ctx.read) {
		ESP_LOGW(TAG, "Response data lost, actual read: %d", ctx.read);
		goto exit;
	}

	ret = ctx.read;

exit:
	esp_http_client_cleanup(client);
	return ret;
}
