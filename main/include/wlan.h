#ifndef __REALEARTH_WLAN__
#define __REALEARTH_WLAN__

#include "common.h"

esp_err_t wlan_init(void);
esp_err_t wlan_connect(char *ssid, char *password);
esp_err_t wlan_disconnect(void);

#endif  // __REALEARTH_WLAN__
