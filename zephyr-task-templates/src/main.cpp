/*
 * Copyright (c) 2015-2016 Wind River Systems, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file C++ Synchronization demo.  Uses basic C++ functionality.
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/sys/printk.h>

#include <zephyr/drivers/led_strip.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/util.h>

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include <System.hpp>

static uint8_t msgforlora[] = {0x04,0x03,0x02,0x01,0x06};
static uint8_t msgforLEDsLEDShow[] = {CMD_LEDs_SHOW,0x03,0x02,0x01,0x06};
static uint8_t msgforLEDsBuzzer[] = {CMD_WORKQUEUE_SONG,0x00,0x02,0x01,0x06};

struct k_thread coop_thread;
K_THREAD_STACK_DEFINE(coop_stack, 256);
struct k_thread sensor_thread;
K_THREAD_STACK_DEFINE(sensor_stack, 256);

void coop_thread_entry(void)
{
	struct k_timer timer;
	uint16_t count = 0;

	k_timer_init(&timer, NULL, NULL);

	while (1) {
		/* wait for main thread to let us have a turn */
		Service::LoRa::Send(msgforlora);
		Service::LEDs::Send(msgforLEDsLEDShow);
		count++;

		if(count == 1024) {
			msgforLEDsBuzzer[1] = 0;
			Service::LEDs::Send(msgforLEDsBuzzer);
			count = 0;
		}

		/* wait a while, then let main thread have a turn */
		k_timer_start(&timer, K_MSEC(60), K_TIMEOUT_ABS_MS(1000));
		k_timer_status_sync(&timer);
	}
}

void sensor_thread_entry(void)
{
	struct k_timer timer;
	uint8_t count;
	k_timer_init(&timer, NULL, NULL);

	while (1) {
		Service::Sensor::Send((uint8_t*)"sensor_thread_entry");
		count++;

		if(count == 255) {
			msgforLEDsBuzzer[1] = 4;
			Service::LEDs::Send(msgforLEDsBuzzer);
			count = 0;
		}
		
		Service::IMU::Send(msgforlora);

		k_timer_start(&timer, K_MSEC(60), K_TIMEOUT_ABS_MS(1000));
		k_timer_status_sync(&timer);
		
	}
}

int main(void)
{
	struct k_timer timer;
	System::Create(); // check zephyrproject/zephyr/samples/kernel/metairq_dispatch

	uint32_t start = k_cycle_get_32();

	LOG_INF("System Start Timestamp %ul", start);

	k_thread_create(&coop_thread, coop_stack, 256,
			(k_thread_entry_t) coop_thread_entry,
			NULL, NULL, NULL, K_PRIO_COOP(7), 0, K_NO_WAIT);

	k_thread_create(&sensor_thread, sensor_stack, 256,
			(k_thread_entry_t) sensor_thread_entry,
			NULL, NULL, NULL, K_PRIO_COOP(7), 0, K_NO_WAIT);

	k_timer_init(&timer, NULL, NULL);

	// Init Done:
	Service::LEDs::Send(msgforLEDsBuzzer);

	while (1) {
		k_timer_start(&timer, K_MSEC(1), K_NO_WAIT);
		k_timer_status_sync(&timer);

		uint32_t time_now = k_cycle_get_32();
		if( std::chrono::milliseconds(time_now-start) 
							> std::chrono::milliseconds(500000000))
		{
			start = time_now;

			LOG_DBG("System time_now Timestamp %ul", time_now - start);
		}
	}

	return 0;
}
