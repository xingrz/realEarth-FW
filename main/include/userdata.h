#ifndef __REALEARTH_USERDATA__
#define __REALEARTH_USERDATA__

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

#endif  // __REALEARTH_USERDATA__
