/*
 * Copyright (c) 2024 fcarraustewart.
 *
 */

#include <Services/LEDs.hpp>

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
	RGB(0x01, 0x00, 0x00), /* red */
	RGB(0x03, 0x00, 0x00), /* red */
	RGB(0x05, 0x00, 0x00), /* red */
	RGB(0x15, 0x00, 0x00), /* red */
	RGB(0x25, 0x00, 0x00), /* red */
	RGB(0x35, 0x00, 0x00), /* red */
	RGB(0x55, 0x00, 0x00), /* red */
	RGB(0x75, 0x00, 0x00), /* red */
	RGB(0x95, 0x00, 0x00), /* red */
	RGB(0xA5, 0x00, 0x00), /* red */
	RGB(0xB5, 0x00, 0x00), /* red */
    RGB(0xC5, 0x00, 0x00), /* red */
    RGB(0xC5, 0x00, 0x00), /* red */
	RGB(0xB5, 0x00, 0x00), /* red */
	RGB(0xA5, 0x00, 0x00), /* red */
	RGB(0x95, 0x00, 0x00), /* red */
	RGB(0x75, 0x00, 0x00), /* red */
	RGB(0x55, 0x00, 0x00), /* red */
	RGB(0x35, 0x00, 0x00), /* red */
	RGB(0x25, 0x00, 0x00), /* red */
	RGB(0x15, 0x00, 0x00), /* red */
	RGB(0x05, 0x00, 0x00), /* red */
	RGB(0x03, 0x00, 0x00), /* red */
	RGB(0x01, 0x00, 0x00), /* red */
	RGB(0x00, 0x00, 0x01), /* zeros */
	RGB(0x00, 0x00, 0x01), /* zeros */
	RGB(0x00, 0x00, 0x01), /* zeros */
	RGB(0x00, 0x00, 0x01), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x01), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x01), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x01), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x01), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
	RGB(0x00, 0x00, 0x00), /* zeros */
};

static struct led_rgb pixels[STRIP_NUM_PIXELS];
static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

size_t 	Service::LEDs::mColor = 0;
int     Service::LEDs::mRc = 0;
bool    Service::LEDs::mBuzzerDriverRunning = 0;
bool    Service::LEDs::mThreadAnalizerDriverRunning = 0;

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

#include <Drivers/BuzzerDriver.hpp>

namespace {
    ZPP_KERNEL_STACK_DEFINE(cWQStack, 4096);
    static zpp::thread buzzer_tid;
}
#include <System.hpp>
#include <Drivers/BleStateMachine.hpp>
#include <hal/WQBackgroundThread.hpp>

static zpp::WQBackgroundThread mDriverWorkerThread;
static BleStateMachine mStateMachine;


static void work_fn(RTOS::BackgroundWorkHelper_t *w) {
    buzzer_thread();
    
    mStateMachine.RaiseEvent(BleStateMachine::Event::Connected);
    mStateMachine.Run();
    mStateMachine.RaiseEvent(BleStateMachine::Event::Disconnected);
    mStateMachine.Run();

    // Driver Played Song: Let Service::LEDs know.
    uint8_t m[5] = {CMD_BUZZER_AVAILABLE, 0x22, 0x00, 0x00, 0x00};
    Service::LEDs::Send(m);
}

void Service::LEDs::Initialize() {
    // #define EVENTS_INTERESTED RTOS::MsgBroker::Event::BLE_Connected , ...
    // System::mMsgBroker::Subscribe<EVENTS_INTERESTED>();    
    LOG_INF("[Service::%s]::%s().", mName, __FUNCTION__);
	InitializeDriver();

    buzzer_init(nullptr);

    // Launch DriverWorkerThread
    mDriverWorkerThread.Initialize(cWQStack().data(), cWQStack().size(), 
                    zpp::thread_prio::coop(7).native_value());

	zpp::this_thread::set_priority(zpp::thread_prio::preempt(4));
    LOG_INF("\t\t\t%s: LEDs Module Initialized correctly.", __FUNCTION__);
}

#include <zephyr/debug/thread_analyzer.h>
#include <zephyr/logging/log_ctrl.h>
#define CMD_THREAD_TRACER_AVAILABLE 0xA3
#define CMD_LOG_PANIC_THREAD_ANALYZER 0x66

static void work_thread_tracer_log_fn(RTOS::BackgroundWorkHelper_t *w)
{
	thread_analyzer_print(0);
    // Driver Printed Tracer: Let Service::LEDs know.
    uint8_t m[5] = {CMD_THREAD_TRACER_AVAILABLE, 0x22, 0x00, 0x00, 0x00};
    Service::LEDs::Send(m);
	// return;
	// zpp::this_thread::sleep_for(std::chrono::milliseconds(5));
	// log_panic();
}


void Service::LEDs::Handle(const uint8_t arg[]) {
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
		case CMD_THREAD_TRACER_AVAILABLE:
		{
			mThreadAnalizerDriverRunning = false;
		}; break;
		case CMD_LOG_PANIC_THREAD_ANALYZER:
		{
            if(mThreadAnalizerDriverRunning == true)
            {
                // Send();
                break;
            }
			
            auto res = mDriverWorkerThread.ScheduleWork(work_thread_tracer_log_fn, zpp::to_timeout(std::chrono::milliseconds(10)));
			if(res.error() != zpp::error_code::k_busy)
				mThreadAnalizerDriverRunning = true;

		}; break;
        case CMD_BUZZER_AVAILABLE:
        {
            mBuzzerDriverRunning = false;
        }; break;
        case CMD_LEDs_SHOW:
        {
            show();        
        }; break;
        case CMD_BUZZER_INITIALIZATION_COMPLETE_SONG:
        {
            LOG_DBG("CMD_BUZZER_INITIALIZATION_COMPLETE_SONG Received");
            
            // Use a workerQueue with worker_thread...
            // Launching the thread here is difficult to manage
            auto res = buzzer_tid.wakeup();
			if(res.has_value() == false)
			{ LOG_ERR("buzzer_tid.wakeup() failed."); }
            
        }; break;
        case CMD_WORKQUEUE_SONG:
        {
            if(mBuzzerDriverRunning == true)
            {
                // Send();
                break;
            }

            switch(arg[1]){
                case 0:
                    play_beep_once();
                    break;
                case 1: 
                    play_funkytown_once();
                    break;
                case 2: 
                    play_golioth_once();
                    break;
                case 3: 
                    play_mario_once();
                    break;
                case 4:
                    play_double_beep_once();
                    break;
                default:
                    break;
            };
            
            auto res = mDriverWorkerThread.ScheduleWork(work_fn, zpp::to_timeout(std::chrono::milliseconds(500)));
			if(res.error() != zpp::error_code::k_busy)
            	mBuzzerDriverRunning = true;

        }; break;
        default:
        {
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
    const char               	_LEDs::mName[] =  "LEDs";
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

    namespace {
    ZPP_KERNEL_STACK_DEFINE(cLEDsStack, 1024);
    template <>
    zpp::thread_data            _LEDs::mTaskControlBlock = zpp::thread_data();
    template <>
    zpp::thread                 _LEDs::mHandle = zpp::thread(
                                        mTaskControlBlock, 
                                        Service::cLEDsStack(), 
                                        RTOS::cThreadAttributes, 
                                        Service::_LEDs::Run
                                    );
    } //https://www.reddit.com/r/cpp/comments/4ukhh5/what_is_the_purpose_of_anonymous_namespaces/#:~:text=The%20purpose%20of%20an%20anonymous,will%20not%20have%20internal%20linkage.
                                    
}