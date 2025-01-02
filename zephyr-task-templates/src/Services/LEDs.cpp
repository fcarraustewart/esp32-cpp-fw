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
bool    Service::LEDs::mBuzzerDriverRunning = 0;

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

extern "C"{
    #include <Drivers/buzzer_output_drv.h>
}
ZPP_KERNEL_STACK_DEFINE(cWQStack, 2048);
static zpp::thread buzzer_tid;

struct k_work_q my_work_q;
//k_work_poll_submit_to_queue
#include <System.hpp>
#include <Drivers/BleStateMachine.hpp>
static BleStateMachine mStateMachine;
static void work_fn(struct k_work *w)
{
    LOG_INF("Run buzzer_thread");
    struct k_work_delayable *dwork = k_work_delayable_from_work(w);
    k_poll_signal s;
    k_poll_event e;
	k_poll_signal_init(&s);
	k_poll_event_init(&e,
			  K_POLL_TYPE_SIGNAL | K_POLL_TYPE_DATA_AVAILABLE,
			  K_POLL_MODE_NOTIFY_ONLY,
			  &s);
    // if(k_poll_signal_raise(&s, NULL) == 0);
    


    //struct work_context *ctx = CONTAINER_OF(dwork, struct work_context,timed_work);
    
    buzzer_thread();
    
    mStateMachine.RaiseEvent(BleStateMachine::Event::Connected);
    mStateMachine.Run();
    mStateMachine.RaiseEvent(BleStateMachine::Event::Disconnected);
    mStateMachine.Run();

    // Driver Played Song: Let Service::LEDs know.
    uint8_t m[5] = {CMD_BUZZER_AVAILABLE, 0x22, 0x00, 0x00, 0x00};
    Service::LEDs::Send(m);
}

/* Register the work handler */
K_WORK_DELAYABLE_DEFINE(work, work_fn);

struct k_work_poll work_poll;

void Service::LEDs::Initialize() {
    // #define EVENTS_INTERESTED RTOS::MsgBroker::Event::BLE_Connected , ...
    // System::mMsgBroker::Subscribe<EVENTS_INTERESTED>();    
    LOG_INF("[Service::%s]::%s().", mName, __FUNCTION__);
	InitializeDriver();
    void * point = nullptr;
    buzzer_init(point);

    // Launch DriverWorkerThread
    k_work_queue_init(&my_work_q);
    k_work_queue_config cfg;
    k_work_queue_start(&my_work_q, 
                    cWQStack().data(), cWQStack().size(), 
                    zpp::thread_prio::coop(7).native_value(),
                    NULL);

    
	k_work_poll_init(&work_poll,
			 work_fn);


    LOG_INF("\t\t\t%s: LEDs Module Initialized correctly.", __FUNCTION__);
        	
}


void Service::LEDs::Handle(const uint8_t arg[]) {
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
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
            LOG_INF("CMD_BUZZER_INITIALIZATION_COMPLETE_SONG Received");
            
            // Use a workerQueue with worker_thread...
            // Launching the thread here is difficult to manage
            buzzer_tid.wakeup();
            
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
            
            int res = k_work_reschedule_for_queue	(&my_work_q, &work, K_MSEC(500));
            LOG_INF("CMD_WORKQUEUE_SONG: Result (%d)", res);

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