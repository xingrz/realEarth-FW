#ifndef _CLOCK_TASK_LCD_
#define _CLOCK_TASK_LCD_

#include "common.h"

void lcd_proc_task(void *arg);
void lcd_draw(uint16_t *pixels);

#endif  // _CLOCK_TASK_LCD_
