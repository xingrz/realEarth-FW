#ifndef __REALEARTH_TASK_LCD__
#define __REALEARTH_TASK_LCD__

#include "common.h"

void lcd_show_offline(void);
void lcd_show_loading(void);
void lcd_show_qrcode(void);
void lcd_draw_bg(uint8_t *jpeg);
void lcd_draw_fg(uint8_t *jpeg);
void lcd_clear_fg(void);

#endif  // __REALEARTH_TASK_LCD__
