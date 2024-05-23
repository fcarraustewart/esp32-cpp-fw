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
        static void Initialize();
        static void Handle(const uint8_t arg[]);
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
    public:
        enum class TimerMode
        {
            OneShot,
            Periodic
        };
        enum class TimerUnit
        {
            ms,
            us,
            s
        };
        enum class TimerState
        {
            Done,
            Idle,
            Running
        };
        struct TimerEvt
        {
            TimerEvt()
                : mMode(TimerMode::OneShot), mUnit(TimerUnit::us), mState(TimerState::Idle), mCountUp(1){}
            TimerMode   mMode;
            TimerUnit   mUnit;
            TimerState  mState;
            uint64_t    mCountUp;
        };
        TimerEvt reboundTimer;  
    };

}

#endif
