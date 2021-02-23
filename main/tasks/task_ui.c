#include "common.h"
#include "tasks.h"
#include "lvgl.h"

void
ui_proc_task(void *arg)
{
	/*Describe the color for the needles*/
	static lv_color_t needle_colors[3];
	needle_colors[0] = LV_COLOR_BLUE;
	needle_colors[1] = LV_COLOR_ORANGE;
	needle_colors[2] = LV_COLOR_PURPLE;

	/*Create a gauge*/
	lv_obj_t *gauge1 = lv_gauge_create(lv_scr_act(), NULL);
	lv_gauge_set_needle_count(gauge1, 3, needle_colors);
	lv_obj_set_pos(gauge1, 0, 0);
	lv_obj_set_size(gauge1, 240, 240);

	/*Set the values*/
	lv_gauge_set_value(gauge1, 0, 10);
	lv_gauge_set_value(gauge1, 1, 20);
	lv_gauge_set_value(gauge1, 2, 30);

	vTaskDelete(NULL);
}
