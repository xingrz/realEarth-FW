#ifndef __REALEARTH_COMMON__
#define __REALEARTH_COMMON__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_sntp.h"
#include "esp_http_client.h"
#include "nvs.h"
#include "nvs_flash.h"

#define WLAN_SSID_LEN 33
#define WLAN_PASSWORD_LEN 64

#include "wlan.h"
#include "hspi.h"
#include "gc9a01.h"
#include "earth.h"
#include "decode_image.h"
#include "blec.h"

#include "tasks.h"
#include "task_lcd.h"
#include "task_wlan.h"

#endif  // __REALEARTH_COMMON__
