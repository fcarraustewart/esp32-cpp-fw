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

#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/util.h>

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include "Services/LEDs.hpp"
#include "Services/BLE.hpp"
#include "Services/Sensor.hpp"

/**
 * @class semaphore the basic pure virtual semaphore class
 */
class semaphore {
public:
	virtual int wait(void) = 0;
	virtual int wait(int timeout) = 0;
	virtual void give(void) = 0;
};

/* specify delay between greetings (in ms); compute equivalent in ticks */
#define SLEEPTIME  500
#define STACKSIZE 1024

struct k_thread coop_thread;
K_THREAD_STACK_DEFINE(coop_stack, STACKSIZE);

/*
 * @class cpp_semaphore
 * @brief Semaphore
 *
 * Class derives from the pure virtual semaphore class and
 * implements it's methods for the semaphore
 */
class cpp_semaphore: public semaphore {
protected:
	struct k_sem _sema_internal;
public:
	cpp_semaphore();
	virtual ~cpp_semaphore() {}
	virtual int wait(void);
	virtual int wait(int timeout);
	virtual void give(void);
};

/*
 * @brief cpp_semaphore basic constructor
 */
cpp_semaphore::cpp_semaphore()
{
	LOG_INF("Create semaphore %p", this);
	k_sem_init(&_sema_internal, 0, K_SEM_MAX_LIMIT);
}

/*
 * @brief wait for a semaphore
 *
 * Test a semaphore to see if it has been signaled.  If the signal
 * count is greater than zero, it is decremented.
 *
 * @return 1 when semaphore is available
 */
int cpp_semaphore::wait(void)
{
	k_sem_take(&_sema_internal, K_FOREVER);
	return 1;
}

/*
 * @brief wait for a semaphore within a specified timeout
 *
 * Test a semaphore to see if it has been signaled.  If the signal
 * count is greater than zero, it is decremented. The function
 * waits for timeout specified
 *
 * @param timeout the specified timeout in ticks
 *
 * @return 1 if semaphore is available, 0 if timed out
 */
int cpp_semaphore::wait(int timeout)
{
	return k_sem_take(&_sema_internal, K_MSEC(timeout));
}

/**
 * @brief Signal a semaphore
 *
 * This routine signals the specified semaphore.
 */
void cpp_semaphore::give(void)
{
	k_sem_give(&_sema_internal);
}

cpp_semaphore sem_main;
cpp_semaphore sem_coop;

void coop_thread_entry(void)
{
	struct k_timer timer;

	k_timer_init(&timer, NULL, NULL);

	while (1) {
		/* wait for main thread to let us have a turn */
		sem_coop.wait();
		if(Service::BLE::send() == 0)
			LEDs::show();
		/* wait a while, then let main thread have a turn */
		k_timer_start(&timer, K_MSEC(1), K_TIMEOUT_ABS_MS(10));
		k_timer_status_sync(&timer);
		sem_main.give();
	}
}

int main(void)
{
	LEDs::init();
	Service::BLE::init();
	Service::Sensor::init();

	struct k_timer timer;

	LOG_INF("%s: device (strip) is ready!", __FUNCTION__);

	k_thread_create(&coop_thread, coop_stack, STACKSIZE,
			(k_thread_entry_t) coop_thread_entry,
			NULL, NULL, NULL, K_PRIO_COOP(7), 0, K_NO_WAIT);
	k_timer_init(&timer, NULL, NULL);

	while (1) {
		/* wait a while, then let coop thread have a turn */
		k_timer_start(&timer, K_MSEC(100), K_NO_WAIT);
		k_timer_status_sync(&timer);
		if(Service::BLE::send() == 0) {
			LEDs::show();
			Service::Sensor::send();
		}
		
		sem_coop.give();
		
		/* Wait for coop thread to let us have a turn */
		sem_main.wait();
	}

	return 0;
}