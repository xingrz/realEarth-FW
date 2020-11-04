#ifndef _CLOCK_USERDATA_
#define _CLOCK_USERDATA_

#include "common.h"

typedef struct {
	char ssid[WLAN_SSID_LEN];
	char password[WLAN_PASSWORD_LEN];
	uint8_t crc;
} userdata_t;

esp_err_t userdata_init(void);
esp_err_t userdata_read(userdata_t *data);
esp_err_t userdata_write(userdata_t *data);
esp_err_t userdata_clear(void);

#endif  // _CLOCK_USERDATA_
