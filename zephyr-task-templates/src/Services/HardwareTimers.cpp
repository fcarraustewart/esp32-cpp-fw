#include "Services/HardwareTimers.hpp"

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(HardwareTimers);

void Service::HardwareTimers::Initialize() {
    // #define EVENTS_INTERESTED RTOS::MsgBroker::Event::BLE_Connected , ...
    // System::mMsgBroker::Subscribe<EVENTS_INTERESTED>();        	
    LOG_INF("%s: HardwareTimers Module Initialized correctly.", __FUNCTION__);
}

void Service::HardwareTimers::Handle(const uint8_t arg[]) {
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
        default:
        {
            //Logger::Log("[Service::%s]::%s():\t%x.\tNYI.", mName.c_str(), __func__, arg[0]);    
        	LOG_INF("%s: Received something! arg[0]=%d", __FUNCTION__, arg[0]);
            LOG_HEXDUMP_DBG(arg, 5, "\t\t\t HardwareTimers msg Buffer values.");
            break;
        }
    };
}

/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using                       _HardwareTimers = RTOS::ActiveObject<Service::HardwareTimers>;

    template <>
    const uint8_t               _HardwareTimers::mName[] =  "HardwareTimers";
    template <>
    uint8_t                     _HardwareTimers::mCountLoops = 0;
    template <>
    const uint8_t               _HardwareTimers::mInputQueueItemLength = 16;
    template <>
    const uint8_t               _HardwareTimers::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t                _HardwareTimers::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize;
    template <>
    char                        _HardwareTimers::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    RTOS::QueueHandle_t         _HardwareTimers::mInputQueue = RTOS::Hal::QueueCreate(
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize,
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueAllocation
                                    );
    template <>
    RTOS::TaskHandle_t          _HardwareTimers::mHandle = k_thread();
    template <>
    uint8_t                     _HardwareTimers::mReceivedMsg[
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemLength
                                    ] = { 0 };
}
