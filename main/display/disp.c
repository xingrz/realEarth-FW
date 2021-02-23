#include "common.h"

#include "lvgl.h"
#include "disp.h"
#include "gc9a01.h"

#define TAG "disp"
#define DISP_DEBUG 0

static void
disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
#if DISP_DEBUG
	ESP_LOGI(TAG, "flush x1:%d x2:%d y1:%d y2:%d", area->x1, area->x2, area->y1, area->y2);
#endif
	gc9a01_draw(area->x1, area->x2, area->y1, area->y2, (uint16_t *)color_p);
	lv_disp_flush_ready(disp_drv);
}

#if DISP_DEBUG
static void
disp_monitor(lv_disp_drv_t *disp_drv, uint32_t time, uint32_t px)
{
	ESP_LOGI(TAG, "%d px refreshed in %d ms\n", px, time);
}
#endif

void
disp_init(void)
{
	gc9a01_init();
#if GC9A01_INIT_BLANK
	gc9a01_fill(0x0000);
#endif

	static lv_color_t buf[GC9A01_BUF_SIZE];
	static lv_disp_buf_t disp_buf;
	lv_disp_buf_init(&disp_buf, buf, NULL, GC9A01_BUF_SIZE);

	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);

	disp_drv.flush_cb = disp_flush;
#if DISP_DEBUG
	disp_drv.monitor_cb = disp_monitor;
#endif
	disp_drv.buffer = &disp_buf;
	lv_disp_drv_register(&disp_drv);
}
