#ifndef __REALEARTH_WLAN__
#define __REALEARTH_WLAN__

#include "common.h"

esp_err_t wlan_init(void);
esp_err_t wlan_connect();
esp_err_t wlan_set(const char *ssid, const char *password);
esp_err_t wlan_reset(void);
bool wlan_configured(char *ssid);

#endif  // __REALEARTH_WLAN__
