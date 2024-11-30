/*
 * Copyright (c) 2024 fcarraustewart.
 *
 */

#include "Services/LEDs.hpp"

#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>

#define STRIP_NODE		DT_ALIAS(led_strip)

#if DT_NODE_HAS_PROP(DT_ALIAS(led_strip), chain_length)
#define STRIP_NUM_PIXELS	DT_PROP(DT_ALIAS(led_strip), chain_length)
#else
#error Unable to determine length of LED strip
#endif

#define LED_STRIP_DELAY_MSEC 500

#define DELAY_TIME K_MSEC(LED_STRIP_DELAY_MSEC)

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(LEDs);

static const struct led_rgb colors[] = {
	RGB(0xff, 0x00, 0x00), /* red */
	RGB(0x00, 0xff, 0x00), /* green */
	RGB(0x00, 0x00, 0xff), /* blue */
	RGB(0x00, 0xff, 0xff), /* yellow */
	RGB(0xff, 0xff, 0xff), /* white */
};

static struct led_rgb pixels[STRIP_NUM_PIXELS];
static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

size_t 	Service::LEDs::mColor = 0;
int     Service::LEDs::mRc = 0;

void Service::LEDs::init(void){
    if (device_is_ready(strip)) {
        LOG_INF("Found LED strip device %s", strip->name);
        memset(&pixels, 0x00, sizeof(pixels));
        mRc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
        if (mRc) {
            LOG_ERR("couldn't update strip: %d", mRc);
        }
    } else {
        LOG_ERR("LED strip device %s is not ready", strip->name);
        return;
    }

}

void Service::LEDs::show(void) {
    for (size_t cursor = 0; cursor < ARRAY_SIZE(pixels); cursor++) {
        memset(&pixels, 0x00, sizeof(pixels));
        memcpy(&pixels[cursor], &colors[mColor], sizeof(struct led_rgb));

        mRc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
        if (mRc) {
            LOG_ERR("couldn't update strip: %d", mRc);
        }
    }

    mColor = (mColor + 1) % ARRAY_SIZE(colors);
};