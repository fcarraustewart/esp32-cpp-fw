#include "System.hpp"
#include "esp32-hal-gpio.h"

/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 * Test:
 * Connect GPIO18(8) with GPIO4
 * Connect GPIO19(9) with GPIO5
 * Generate pulses on GPIO18(8)/19(9), that triggers interrupt on GPIO4/5
 *
 */

#define GPIO_OUTPUT_IO_0    GPIO_NUM_17
#define GPIO_OUTPUT_IO_1    GPIO_NUM_16
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))
/*
 * Let's say, GPIO_OUTPUT_IO_0=18, GPIO_OUTPUT_IO_1=19
 * In binary representation,
 * 1ULL<<GPIO_OUTPUT_IO_0 is equal to 0000000000000000000001000000000000000000 and
 * 1ULL<<GPIO_OUTPUT_IO_1 is equal to 0000000000000000000010000000000000000000
 * GPIO_OUTPUT_PIN_SEL                0000000000000000000011000000000000000000
 * */
#define GPIO_INPUT_IO_0     GPIO_NUM_34
#define GPIO_INPUT_IO_1     GPIO_NUM_19
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
#define ESP_INTR_FLAG_DEFAULT 0

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    static uint8_t messageForinternalGPIOs[6]={0x00, 0x00, 0x00, 0x00, 0x00};
    uint32_t gpio_num = (uint32_t) arg;

    messageForinternalGPIOs[0]=BUTTON_EVT_CMD;
    messageForinternalGPIOs[1]=(uint8_t)gpio_num;
    Service::internalGPIOs::Send((uint8_t *)&messageForinternalGPIOs);
}

void Service::internalGPIOs::Initialize()
{
    // #define EVENTS_INTERESTED RTOS::MsgBroker::Event::BLE_Connected , ...
    // System::mMsgBroker::Subscribe<EVENTS_INTERESTED>();
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf);

    //change gpio interrupt type for one pin
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);

}

void Service::internalGPIOs::Handle(const uint8_t arg[]){
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
        case REBOUND_TIMEUP_CMD:
        {
            //// In order to turn it back on: hook isr handler for specific gpio pin again
            gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
            gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);
            Logger::Log("[Service::%s]::%s().\t Got REBOUND_TIMEUP_CMD.", mName.c_str(), __func__);

            // Unsubscribe off of the rebound timer Timer100us topic HardwareTimers
        }   break;
        case BUTTON_EVT_CMD:
        {
            Logger::Log("[Service::%s]::%s():\t%x.\t Got button interrupt. Will request a rebound timer.", mName.c_str(), __func__, arg[0]);
            uint8_t ButtonID = arg[1];
            switch (ButtonID)
            {
                case GPIO_INPUT_IO_0:
                {
                    Logger::Log("[Service::%s]::%s().\t Got button 0x%x. Disabling ISR for this button.", mName.c_str(), __func__, GPIO_INPUT_IO_0);

                    //remove isr handler for gpio number.
                    gpio_isr_handler_remove(GPIO_INPUT_IO_0);
                    // Can we empty fifo queue mQueue?
                    UBaseType_t queueSize = uxQueueMessagesWaiting(mInputQueue);
                    if(queueSize != 0)
                        xQueueReset(mInputQueue);

                }   break;
                case GPIO_INPUT_IO_1:
                {
                    Logger::Log("[Service::%s]::%s().\t Got button %x. Disabling ISR for this button.", mName.c_str(), __func__, GPIO_INPUT_IO_1);

                    //remove isr handler for gpio number.
                    gpio_isr_handler_remove(GPIO_INPUT_IO_1);
                    // Can we empty fifo queue mQueue?
                    UBaseType_t queueSize = uxQueueMessagesWaiting(mInputQueue);
                    if(queueSize != 0)
                        xQueueReset(mInputQueue);
                    
                }   break;
                
                default:
                    break;
            }

            Logger::Log("[Service::%s]::%s().\t Requesting HwTimers Rebound Timer = 100us.", mName.c_str(), __func__);
            
            uint8_t                             msgRequestReboundTimer[RTOS::MsgBroker::cMaxPayloadLength]; //RTOS::MsgBroker::payload_t
            Service::HardwareTimers::TimerEvt*  timer_requested =   new Service::HardwareTimers::TimerEvt();
            Message*                            topic_timer_100us = new Message("Timer100us", "HardwareTimers");

            msgRequestReboundTimer[0]   = REQUEST_TIMER_MSG_PUBLISHED_CMD;
            uint64_t mCountUp   = 2;
            uint8_t mMode      = (uint8_t)Service::HardwareTimers::TimerMode::OneShot;
            uint8_t mState     = (uint8_t)Service::HardwareTimers::TimerState::Idle;
            uint8_t mUnit      = (uint8_t)Service::HardwareTimers::TimerUnit::us;
            
            int err = topic_timer_100us->addEventData("Cmd",    std::to_string(REQUEST_TIMER_MSG_PUBLISHED_CMD));
            err = topic_timer_100us->addEventData("mCountUp",   std::to_string(mCountUp));
            err = topic_timer_100us->addEventData("mMode",      std::to_string(mMode));
            err = topic_timer_100us->addEventData("mState",     std::to_string(mState));
            err = topic_timer_100us->addEventData("mUnit",      std::to_string(mUnit));
            err = topic_timer_100us->addEventData("ButtonID",   std::to_string(arg[1]));
            
            size_t size = topic_timer_100us->serialize(msgRequestReboundTimer+1, sizeof(msgRequestReboundTimer)-1);

            delete timer_requested;
            delete topic_timer_100us;

            Logger::Log("[Service::%s]::%s().\t Subscribe to mTopic 100us timers.", mName.c_str(), __func__);
            System::mMsgBroker.mIPC.subscribeTo("Timer100us", [&ButtonID](const Message& message) {
                Logger::Log("[Service::%s]: Subscriber received message.", mName.c_str());   
                try {
                    // thought: Message var is not actually needed from inside this lambda. Since you can capture all the info needed.
                    uint8_t msgReboundTimerDone[Service::internalGPIOs::mInputQueueItemSize] = {REBOUND_TIMEUP_CMD, ButtonID};
                    Service::internalGPIOs::Send(msgReboundTimerDone);

                    // Unsubscribe from following messages. / >?
                } catch (const std::bad_any_cast&) {
                    Logger::Log("[Service::%s].\t Bad any cast inside lambda function subscribeTo().", mName.c_str());    
                }
            });

            // Send Msg
            if (size > 0) {
                Service::HardwareTimers::Send(msgRequestReboundTimer);
            } else
            {   
                if(err==0)
                    Logger::Log("[Service::%s].\t Good addEventData().", mName.c_str());   
                Logger::Log("[Service::%s].\t Error SENDING Service::HardwareTimers::Send(msgRequestReboundTimer).", mName.c_str());   
            }

            
        } break;
        default:
        {
            Logger::Log("[Service::%s]::%s():\t%x.\tNYI.", mName.c_str(), __func__, arg[0]);
            break;
        }
    };
}

/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using               _internalGPIOs = RTOS::ActiveObject<Service::internalGPIOs>;

    template <>
    const std::string   _internalGPIOs::mName = std::string("internalGPIOs");
    template <>
    uint8_t             _internalGPIOs::mCountLoops = 0;
    template <>
    const uint8_t       _internalGPIOs::mInputQueueItemLength = 16;
    template <>
    const uint8_t       _internalGPIOs::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t        _internalGPIOs::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::internalGPIOs>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::internalGPIOs>::mInputQueueItemSize;
    template <>
    uint8_t             _internalGPIOs::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::internalGPIOs>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    QueueHandle_t       _internalGPIOs::mInputQueue = xQueueCreate(
                                        RTOS::ActiveObject<Service::internalGPIOs>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::internalGPIOs>::mInputQueueItemSize
                                    );
    template <>
    TaskHandle_t        _internalGPIOs::mHandle = 0;
    template <>
    uint8_t             _internalGPIOs::mReceivedMsg[
                                        RTOS::ActiveObject<Service::internalGPIOs>::mInputQueueItemSize
                                    ] = { 0 };
}
