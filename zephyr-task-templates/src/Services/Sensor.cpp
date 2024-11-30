/*
 * Copyright (c) 2024 fcarraustewart, SAS.
 *
 */
#include "Services/Sensor.hpp"

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(Sensor);

/**********************************/
/* samples/sensor/vcnl4040 */
#include <zephyr/kernel.h>

#include <zephyr/sys/printk.h>
#include <zephyr/sys_clock.h>
#include <stdio.h>

#define MAX_TEST_TIME	30
#define SLEEPTIME	10

/**********************************/

static void print_proxy_data(const struct device *dev)
{
	struct sensor_value pdata;

	if (sensor_channel_get(dev, SENSOR_CHAN_PROX, &pdata) < 0) {
		LOG_INF("Cannot read proximity data.\n");
		return;
	}

	LOG_DBG("Proximity: %d\n", (uint16_t) pdata.val1);
}
#if defined(CONFIG_VCNL4040_ENABLE_ALS)
static void print_als_data(const struct device *dev)
{
	struct sensor_value val;

	if (sensor_channel_get(dev, SENSOR_CHAN_LIGHT, &val) < 0) {
		LOG_INF("ALS read error.\n");
		return;
	}

	LOG_DBG("Light (lux): %d\n", (uint16_t) val.val1);
}
#endif

//const struct device *const Service::Sensor::mVCNL = DEVICE_DT_GET_ONE(vishay_vcnl4040);

int Service::Sensor::init(void)
{
	if (!device_is_ready(mVCNL)) {
		printk("sensor: device not ready.\n");
		return 0;
	}

	return 0;
}

int Service::Sensor::send(void)
{
	if (sensor_sample_fetch(mVCNL) < 0) {
		LOG_INF("sample update error.\n");
	} else {
		print_proxy_data(mVCNL);
#if defined(CONFIG_VCNL4040_ENABLE_ALS)
		print_als_data(mVCNL);
#endif
	}

    return 0;
}