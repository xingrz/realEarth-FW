#ifndef _CLOCK_COMMON_
#define _CLOCK_COMMON_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_system.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "wlan.h"
#include "hspi.h"
#include "gc9a01.h"

#include "task_lcd.h"
#include "task_btn.h"
#include "task_wlan.h"

// #define BIT(NB) (1 << (NB))
#define BITS(HB, LB) ((2 << (HB)) - (1 << (LB)))

#define BOOT_TASK_LCD BIT(0)
#define BOOT_TASK_BTN BIT(1)
#define BOOT_TASK_WLAN BIT(2)
#define BOOT_TASK_ALL BITS(2, 0)

#endif  // _CLOCK_COMMON_
