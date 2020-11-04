#ifndef _CLOCK_TASK_LCD_
#define _CLOCK_TASK_LCD_

#include "common.h"

void lcd_proc_task(void *arg);
void lcd_init(void);
void lcd_show_loading(void);
void lcd_show_qrcode(void);
void lcd_draw_bg(uint8_t *jpeg);
void lcd_draw_fg(uint8_t *jpeg);
void lcd_clear_fg(void);

#endif  // _CLOCK_TASK_LCD_
