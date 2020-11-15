#ifndef _CLOCK_WLAN_
#define _CLOCK_WLAN_

#include "common.h"

esp_err_t wlan_init(void);
esp_err_t wlan_connect(char *ssid, char *password);
esp_err_t wlan_disconnect(void);

#endif  // _CLOCK_WLAN_
