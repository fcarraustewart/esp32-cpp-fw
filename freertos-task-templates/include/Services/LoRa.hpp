#ifndef SERVICE_LORA__H_H
#define SERVICE_LORA__H_H
#include "ActiveObject.hpp"
/**
 * Customize the static methods of an RTOS::ActiveObject
 */
namespace Service
{
    class LoRa : public RTOS::ActiveObject<LoRa>
    {
    public:
        static void Initialize(){};
        static void Handle(const uint8_t arg[]);
        static void End(){
        };

        LoRa() : RTOS::ActiveObject<LoRa>(){};
    private:
    };

}
#endif
