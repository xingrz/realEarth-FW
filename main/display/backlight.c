#include "common.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "pinout.h"
#include "backlight.h"

#define TAG "backlight"

#define BLK_LEDC_TIMER LEDC_TIMER_0
#define BLK_LEDC_MODE LEDC_HIGH_SPEED_MODE
#define BLK_LEDC_CHANNEL LEDC_CHANNEL_0
#define BLK_LEDC_RES LEDC_TIMER_7_BIT

#define BLK_FADE_TIME 500

static uint16_t blk_steps[BACKLIGHT_MAX + 1] = {2, 15, 31, 63, 127};
static uint16_t blk_level = BACKLIGHT_MAX;

void
backlight_init(void)
{
	ledc_timer_config_t timer = {
			.duty_resolution = BLK_LEDC_RES,
			.freq_hz = 5000,
			.speed_mode = BLK_LEDC_MODE,
			.timer_num = BLK_LEDC_TIMER,
			.clk_cfg = LEDC_AUTO_CLK,
	};

	ledc_timer_config(&timer);

	ledc_channel_config_t channel = {
			.channel = BLK_LEDC_CHANNEL,
			.duty = 0,
			.gpio_num = PIN_BLK,
			.speed_mode = BLK_LEDC_MODE,
			.hpoint = 0,
			.timer_sel = BLK_LEDC_TIMER,
	};

	ledc_channel_config(&channel);

	ledc_fade_func_install(0);
}

uint8_t
backlight_get(void)
{
	return blk_level;
}

void
backlight_set(uint8_t level)
{
	ESP_LOGI(TAG, "Set backlight: %d", level);
	ledc_set_fade_with_time(BLK_LEDC_MODE, BLK_LEDC_CHANNEL, blk_steps[level], BLK_FADE_TIME);
	ledc_fade_start(BLK_LEDC_MODE, BLK_LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
	blk_level = level;
}
