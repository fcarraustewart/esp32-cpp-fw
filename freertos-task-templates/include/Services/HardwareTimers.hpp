#ifndef SERVICE_HARDWARE_TIMERS__H_H
#define SERVICE_HARDWARE_TIMERS__H_H
#include "ActiveObject.hpp"
/**
 * Customize the static methods of an RTOS::ActiveObject
 */
namespace Service
{
    class HardwareTimers : public RTOS::ActiveObject<HardwareTimers>
    {
    public:
        static void Initialize(){};
        static void Handle(const uint8_t arg[]){
            /**
             * Handle arg packet.
             */
            switch(arg[0])
            {
                case 5:
                {
                    Logger::Log("[Service::%s]::%s():\t%x. Pass to LoRa.", mName.c_str(), __func__, arg[0]);
                    //Service::LoRa::Send(arg);
                    break;
                }
                default:
                {
                    Logger::Log("[Service::%s]::%s():\t%x.\tNYI.", mName.c_str(), __func__, arg[0]);
                    break;
                }
            };
        };
        static void End(){
        };

        HardwareTimers() : RTOS::ActiveObject<HardwareTimers>(){};
    private:
    };

}
/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using               _HardwareTimers = RTOS::ActiveObject<Service::HardwareTimers>;

    template <>
    const std::string   _HardwareTimers::mName = std::string("HardwareTimers");
    template <>
    uint8_t             _HardwareTimers::mCountLoops = 0;
    template <>
    const uint8_t       _HardwareTimers::mInputQueueItemLength = 16;
    template <>
    const uint8_t       _HardwareTimers::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t        _HardwareTimers::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize;
    template <>
    uint8_t             _HardwareTimers::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    QueueHandle_t       _HardwareTimers::mInputQueue = xQueueCreate(
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize
                                    );
    template <>
    TaskHandle_t        _HardwareTimers::mHandle = 0;
    template <>
    uint8_t             _HardwareTimers::mReceivedMsg[
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize
                                    ] = { 0 };
}

#endif
