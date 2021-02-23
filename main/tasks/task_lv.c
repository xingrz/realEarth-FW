#include "common.h"
#include "tasks.h"
#include "lvgl.h"

void
lv_proc_task(void *arg)
{
	while (1) {
		lv_task_handler();
		vTaskDelay(30 / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}
