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

static uint8_t msgforLEDsBuzzer[] = {CMD_WORKQUEUE_SONG,0x00,0x02,0x01,0x06};

#include <Drivers/EdgeImpulse.hpp>

int main(void)
{
	System::Create(); // check zephyrproject/zephyr/samples/kernel/metairq_dispatch

	uint32_t start = k_cycle_get_32();

	LOG_INF("System Start Timestamp %ul", start);
	// Init Done:
	zpp::this_thread::sleep_for(std::chrono::milliseconds(2000));

	Service::LEDs::Send(msgforLEDsBuzzer);
	Service::IMU::Send(msgforLEDsBuzzer);

	while (1) {

		uint32_t time_now = k_cycle_get_32();
		if( (std::chrono::milliseconds(time_now-start) 
							> std::chrono::milliseconds(5000))
		)
		{
			start = time_now;

			LOG_DBG("System time_now Timestamp %ul", time_now - start);
		}

		EImpulse::Run();
		zpp::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}
