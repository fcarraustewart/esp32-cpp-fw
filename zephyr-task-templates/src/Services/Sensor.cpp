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

int Service::Sensor::InitializeDriver(void)
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

void Service::Sensor::Initialize() {
    // #define EVENTS_INTERESTED RTOS::MsgBroker::Event::BLE_Connected , ...
    // System::mMsgBroker::Subscribe<EVENTS_INTERESTED>();
	if(!InitializeDriver())        	
    	LOG_INF("%s: Sensor Module Initialized correctly.", __FUNCTION__);
}

void Service::Sensor::Handle(const uint8_t arg[]) {
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
        default:
        {
			Service::Sensor::send();
            LOG_DBG("[Service::%s]::%s():\t%x.\tNYI.", mName, __func__, arg[0]);   
            LOG_HEXDUMP_DBG(arg, 5, "\t\t\t Sensor msg Buffer values.");
            break;
        }
    };
}

/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using                       _Sensor = RTOS::ActiveObject<Service::Sensor>;

    template <>
    const uint8_t               _Sensor::mName[] =  "Sensor";
    template <>
    uint8_t                     _Sensor::mCountLoops = 0;
    template <>
    const uint8_t               _Sensor::mInputQueueItemLength = 16;
    template <>
    const uint8_t               _Sensor::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t                _Sensor::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::Sensor>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::Sensor>::mInputQueueItemSize;
    template <>
    char                        _Sensor::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::Sensor>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    RTOS::QueueHandle_t         _Sensor::mInputQueue = RTOS::Hal::QueueCreate(
                                        RTOS::ActiveObject<Service::Sensor>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::Sensor>::mInputQueueItemSize,
                                        RTOS::ActiveObject<Service::Sensor>::mInputQueueAllocation
                                    );
    template <>
    uint8_t                     _Sensor::mReceivedMsg[
                                        RTOS::ActiveObject<Service::Sensor>::mInputQueueItemLength
                                    ] = { 0 };


    ZPP_KERNEL_STACK_DEFINE(cSensorThreadStack, 512);
    template <>
    zpp::thread_data            _Sensor::mTaskControlBlock = zpp::thread_data();
    template <>
    zpp::thread                 _Sensor::mHandle = zpp::thread(
                                        mTaskControlBlock, 
                                        Service::cSensorThreadStack(), 
                                        RTOS::cThreadAttributes, 
                                        Service::_Sensor::Run
                                    );


                                    
}
