#include <Services/LoRa.hpp>

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(LoRa);

void Service::LoRa::Initialize() {
    LOG_INF("%s: LoRa Module Initialized correctly.", __FUNCTION__);
	zpp::this_thread::set_priority(zpp::thread_prio::preempt(2));
}

void Service::LoRa::Handle(const uint8_t arg[]) {
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
        default:
        {
            LOG_DBG("[Service::%s]::%s():\t%x.\tNYI.", mName, __func__, arg[0]);    
            LOG_HEXDUMP_DBG(arg, 5, "\t\t\t LoRa msg Buffer values.");
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
    uint8_t                     _LoRa::mReceivedMsg[
                                        RTOS::ActiveObject<Service::LoRa>::mInputQueueItemLength
                                    ] = { 0 };


    namespace {
    ZPP_KERNEL_STACK_DEFINE(lorastack, 1024);
    template <>
    zpp::thread_data            _LoRa::mTaskControlBlock = zpp::thread_data();
    template <>
    zpp::thread                 _LoRa::mHandle = zpp::thread(
                                        mTaskControlBlock, 
                                        Service::lorastack(), 
                                        RTOS::cThreadAttributes, 
                                        Service::_LoRa::Run
                                    );
    } //https://www.reddit.com/r/cpp/comments/4ukhh5/what_is_the_purpose_of_anonymous_namespaces/#:~:text=The%20purpose%20of%20an%20anonymous,will%20not%20have%20internal%20linkage.
}
