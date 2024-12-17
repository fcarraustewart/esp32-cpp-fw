/*
 * Copyright (c) 2024 fcarraustewart.
 *
 */

#include "Services/LEDs.hpp"

#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/util.h>

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
	RGB(0x35, 0x14, 0x25), /* red */
	RGB(0x25, 0x35, 0x14), /* green */
	RGB(0x14, 0x25, 0x35), /* blue */
	RGB(0x25, 0x30, 0x35), /* yellow */
	RGB(0x10, 0x20, 0x35), /* white */
};

static struct led_rgb pixels[STRIP_NUM_PIXELS];
static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

size_t 	Service::LEDs::mColor = 0;
int     Service::LEDs::mRc = 0;

void Service::LEDs::InitializeDriver(void){
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

    mColor = (mColor + 4) % ARRAY_SIZE(colors);
};


void Service::LEDs::Initialize() {
    // #define EVENTS_INTERESTED RTOS::MsgBroker::Event::BLE_Connected , ...
    // System::mMsgBroker::Subscribe<EVENTS_INTERESTED>();    
    LOG_INF("[Service::%s]::%s().", mName, __FUNCTION__);
	InitializeDriver();
    LOG_INF("\t\t\t%s: LEDs Module Initialized correctly.", __FUNCTION__);
        	
}

void Service::LEDs::Handle(const uint8_t arg[]) {
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
        default:
        {
            show();
            LOG_DBG("[Service::%s]::%s():\t%x.\tNYI.", mName, __func__, arg[0]);   
            LOG_HEXDUMP_DBG(arg, 5, "\t\t\t LEDs msg Buffer values.");
            break;
        }
    };
}

/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using                       _LEDs = RTOS::ActiveObject<Service::LEDs>;

    template <>
    const uint8_t               _LEDs::mName[] =  "LEDs";
    template <>
    uint8_t                     _LEDs::mCountLoops = 0;
    template <>
    const uint8_t               _LEDs::mInputQueueItemLength = 16;
    template <>
    const uint8_t               _LEDs::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t                _LEDs::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::LEDs>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::LEDs>::mInputQueueItemSize;
    template <>
    char                        _LEDs::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::LEDs>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    RTOS::QueueHandle_t         _LEDs::mInputQueue = RTOS::Hal::QueueCreate(
                                        RTOS::ActiveObject<Service::LEDs>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::LEDs>::mInputQueueItemSize,
                                        RTOS::ActiveObject<Service::LEDs>::mInputQueueAllocation
                                    );
    template <>
    uint8_t                     _LEDs::mReceivedMsg[
                                        RTOS::ActiveObject<Service::LEDs>::mInputQueueItemLength
                                    ] = { 0 };


    ZPP_KERNEL_STACK_DEFINE(cLEDsStack, 512);
    template <>
    zpp::thread_data            _LEDs::mTaskControlBlock = zpp::thread_data();
    template <>
    zpp::thread                 _LEDs::mHandle = zpp::thread(
                                        mTaskControlBlock, 
                                        Service::cLEDsStack(), 
                                        RTOS::cThreadAttributes, 
                                        Service::_LEDs::Run
                                    );


                                    
}