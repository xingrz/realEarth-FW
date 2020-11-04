#ifndef _CLOCK_TASK_WLAN_
#define _CLOCK_TASK_WLAN_

#include "common.h"

void wlan_proc_task(void *arg);
void wlan_setup(char *ssid, char *password);
void wlan_reset(void);

#endif  // _CLOCK_TASK_WLAN_
