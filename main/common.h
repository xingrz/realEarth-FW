#ifndef _CLOCK_COMMON_
#define _CLOCK_COMMON_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

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
#include "userdata.h"

#include "task_lcd.h"
#include "task_btn.h"
#include "task_wlan.h"
#include "task_ntp.h"
#include "task_earth.h"
#include "task_ble.h"

// #define BIT(NB) (1 << (NB))
#define BITS(HB, LB) ((2 << (HB)) - (1 << (LB)))

#define BOOT_TASK_LCD BIT(0)
#define BOOT_TASK_BTN BIT(1)
#define BOOT_TASK_WLAN BIT(2)
#define BOOT_TASK_NTP BIT(3)
#define BOOT_TASK_EARTH BIT(4)
#define BOOT_TASK_BLE BIT(5)
#define BOOT_TASK_ALL BITS(5, 0)

#endif  // _CLOCK_COMMON_
