#ifndef __REALEARTH_TASK_WLAN__
#define __REALEARTH_TASK_WLAN__

#include "common.h"

void wlan_setup(char *ssid, char *password);
void wlan_reset(void);
bool wlan_configured(void);

#endif  // __REALEARTH_TASK_WLAN__
