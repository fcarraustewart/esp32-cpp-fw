#ifndef SERVICE_HARDWARE_TIMERS__H_H
#define SERVICE_HARDWARE_TIMERS__H_H
#include "ActiveObject.hpp"
#include "esp32-hal-timer.h"

/**
 * Customize the static methods of an RTOS::ActiveObject
 */
namespace Service
{
    class HardwareTimers : public RTOS::ActiveObject<HardwareTimers>
    {
    public:
        static void Initialize()
        {
            mTimer = timerBegin(0, 2, true);
            timerAttachInterrupt(mTimer, &Service::HardwareTimers::OnTimer, true);
            timerAlarmWrite(mTimer, 1000000, true);
            timerAlarmEnable(mTimer);
        };
        static void Handle(const uint8_t arg[]){
            /**
             * Handle arg packet.
             */
            switch(arg[0])
            {
                case 5:
                {
                    Logger::Log("[Service::%s]::%s():\t%x. Pass to LoRa.", mName.c_str(), __func__, arg[0]);
                    Service::LoRa::Send(arg);
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
        static void OnTimer()
        {
            Service::HardwareTimers::Send((uint8_t *)&(++mInterruptCounter)); /**< This runs the xQueueFromISR version of Send. */
        };

        HardwareTimers() : RTOS::ActiveObject<HardwareTimers>(){};
    private:
        static volatile uint16_t    mInterruptCounter;
        static hw_timer_t           *mTimer;

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

/** 
 * Build specific Service static members
 *  Otherwise it complains that the RTOS 
 * activeObject does not have these members, which is true.
*/
namespace Service
{
    volatile uint16_t   HardwareTimers::mInterruptCounter = 0;
    hw_timer_t*         HardwareTimers::mTimer = nullptr;
}


#endif
