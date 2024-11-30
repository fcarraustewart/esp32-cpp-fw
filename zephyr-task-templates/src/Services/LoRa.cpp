#include "Services/LoRa.hpp"

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(LoRa);

void Service::LoRa::Initialize() {
    // #define EVENTS_INTERESTED RTOS::MsgBroker::Event::BLE_Connected , ...
    // System::mMsgBroker::Subscribe<EVENTS_INTERESTED>();        	
    LOG_INF("%s: LoRa Module Initialized correctly.", __FUNCTION__);
}

void Service::LoRa::Handle(const uint8_t arg[]) {
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
        default:
        {
            //Logger::Log("[Service::%s]::%s():\t%x.\tNYI.", mName.c_str(), __func__, arg[0]);    
        	LOG_INF("%s: device (strip) is ready!", __FUNCTION__);
            break;
        }
    };
}

/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using                       _LoRa = RTOS::ActiveObject<Service::LoRa>;

    template <>
    const uint8_t               _LoRa::mName[] =  "LoRa";
    template <>
    uint8_t                     _LoRa::mCountLoops = 0;
    template <>
    const uint8_t               _LoRa::mInputQueueItemLength = 16;
    template <>
    const uint8_t               _LoRa::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t                _LoRa::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::LoRa>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::LoRa>::mInputQueueItemSize;
    template <>
    char                        _LoRa::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::LoRa>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    RTOS::QueueHandle_t         _LoRa::mInputQueue = RTOS::Hal::QueueCreate(
                                        RTOS::ActiveObject<Service::LoRa>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::LoRa>::mInputQueueItemSize,
                                        RTOS::ActiveObject<Service::LoRa>::mInputQueueAllocation
                                    );
    template <>
    RTOS::TaskHandle_t          _LoRa::mHandle = k_thread();
    template <>
    uint8_t                     _LoRa::mReceivedMsg[
                                        RTOS::ActiveObject<Service::LoRa>::mInputQueueItemSize
                                    ] = { 0 };
}
