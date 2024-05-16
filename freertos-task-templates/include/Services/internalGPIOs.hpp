#ifndef SERVICE_internalGPIOs__H_H
#define SERVICE_internalGPIOs__H_H
#include "ActiveObject.hpp"
/**
 * Customize the static methods of an RTOS::ActiveObject
 */
namespace Service
{
    class internalGPIOs : public RTOS::ActiveObject<internalGPIOs>
    {
    public:
        static void Initialize();
        static void Handle(const uint8_t arg[]);
        static void End(){
        };

        internalGPIOs() : RTOS::ActiveObject<internalGPIOs>(){};
    private:
    };

}
#endif
