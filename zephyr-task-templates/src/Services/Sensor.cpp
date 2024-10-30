/*
 * Copyright (c) 2024 fcarraustewart, SAS.
 *
 */
#include "Services/Sensor.hpp"

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(Sensor);

/**********************************/
/* samples/sensor/vcnl4040 */
#include <zephyr/kernel.h>

#include <zephyr/sys/printk.h>
#include <zephyr/sys_clock.h>
#include <stdio.h>

#define MAX_TEST_TIME	5000
#define SLEEPTIME	300

/**********************************/

static void print_proxy_data(const struct device *dev)
{
	struct sensor_value pdata;

	if (sensor_channel_get(dev, SENSOR_CHAN_PROX, &pdata) < 0) {
		LOG_INF("Cannot read proximity data.\n");
		return;
	}

	LOG_INF("Proximity: %d\n", (uint16_t) pdata.val1);
}
#if defined(CONFIG_VCNL4040_ENABLE_ALS)
static void print_als_data(const struct device *dev)
{
	struct sensor_value val;

	if (sensor_channel_get(dev, SENSOR_CHAN_LIGHT, &val) < 0) {
		LOG_INF("ALS read error.\n");
		return;
	}

	LOG_INF("Light (lux): %d\n", (uint16_t) val.val1);
}
#endif
static void test_polling_mode(const struct device *dev)
{
	int32_t remaining_test_time = MAX_TEST_TIME;

	do {
		if (sensor_sample_fetch(dev) < 0) {
			LOG_INF("sample update error.\n");
		} else {
			print_proxy_data(dev);
#if defined(CONFIG_VCNL4040_ENABLE_ALS)
			print_als_data(dev);
#endif
		}

		/* wait a while */
		k_sleep(K_MSEC(SLEEPTIME));

		remaining_test_time -= SLEEPTIME;
	} while (remaining_test_time > 0);
}

#if defined(CONFIG_VCNL4040_TRIGGER)
static void trigger_handler(const struct device *dev,
			    const struct sensor_trigger *trig)
{
	switch (trig->type) {
	case SENSOR_TRIG_THRESHOLD:
		LOG_INF("Triggered.\n");
		if (sensor_sample_fetch(dev) < 0) {
			LOG_INF("sample update error.\n");
		} else {
			print_proxy_data(dev);
		}
		return;

	default:
		LOG_INF("trigger handler: unknown trigger type.\n");
		return;
	}
}
#endif

static void test_trigger_mode(const struct device *dev)
{
#if defined(CONFIG_VCNL4040_TRIGGER)
	struct sensor_trigger trig;
	struct sensor_value attr;

	LOG_INF("Testing proximity trigger.\n");

	attr.val1 = 127;
	attr.val2 = 0;

	if (sensor_attr_set(dev, SENSOR_CHAN_PROX,
			    SENSOR_ATTR_UPPER_THRESH, &attr) < 0) {
		LOG_INF("cannot set proximity high threshold.\n");
		return;
	}

	attr.val1 = 122;

	if (sensor_attr_set(dev, SENSOR_CHAN_PROX,
			    SENSOR_ATTR_LOWER_THRESH, &attr) < 0) {
		LOG_INF("cannot set proximity low threshold.\n");
		return;
	}

	trig.type = SENSOR_TRIG_THRESHOLD;
	trig.chan = SENSOR_CHAN_PROX;

	if (sensor_trigger_set(dev, &trig, trigger_handler) < 0) {
		LOG_INF("cannot set trigger.\n");
		return;
	}

	k_sleep(K_MSEC(MAX_TEST_TIME));

	if (sensor_trigger_set(dev, &trig, NULL) < 0) {
		LOG_INF("cannot clear trigger.\n");
		return;
	}

	LOG_INF("Threshold trigger test finished.\n");
#endif

}

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
	LOG_INF("Testing the polling mode.\n");
	test_polling_mode(mVCNL);
	LOG_INF("Polling mode test finished.\n");

	LOG_INF("Testing the trigger mode DUMMY.\n");
	// interrupt poll not implemented yet: test_trigger_mode(vcnl);
    return 0;
}