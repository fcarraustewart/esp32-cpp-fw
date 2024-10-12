#include "System.hpp"
#include "esp32-hal-gpio.h"

#define ESP_INTR_FLAG_DEFAULT 0

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint8_t messageForinternalGPIOs[Service::internalGPIOs::mInputQueueItemSize];
    uint32_t gpio_num = (uint32_t) arg;

    messageForinternalGPIOs[0]=BUTTON_EVT_CMD;
    messageForinternalGPIOs[1]=(uint8_t)gpio_num;
    Service::internalGPIOs::Send((uint8_t *)&messageForinternalGPIOs);
}

void Service::internalGPIOs::Initialize()
{
}

void Service::internalGPIOs::Handle(const uint8_t arg[]){
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
        default:
        break;
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
