#include "System.hpp"
#include "esp32-hal-gpio.h"

void Service::internalGPIOs::Initialize()
{
    // #define EVENTS_INTERESTED RTOS::MsgBroker::Event::BLE_Connected , ...
    // System::mMsgBroker::Subscribe<EVENTS_INTERESTED>();
}

void Service::internalGPIOs::Handle(const uint8_t arg[]){
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
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
