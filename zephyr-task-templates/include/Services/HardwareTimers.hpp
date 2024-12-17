#ifndef SERVICE_HARDWARETIMERS__H_H
#define SERVICE_HARDWARETIMERS__H_H
#include "ActiveObject.hpp"
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

        HardwareTimers() : RTOS::ActiveObject<HardwareTimers>(){};
    private:
    };

}
#endif
