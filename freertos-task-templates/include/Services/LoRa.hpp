#ifndef SERVICE_LORA__H_H
#define SERVICE_LORA__H_H
#include "ActiveObject.hpp"

/**
* Customize the static methods of an RTOS::ActiveObject
*/
namespace Service{
    class LoRa : public RTOS::ActiveObject<LoRa>
    {
    public:
        static void Initialize() 
        { 
        };
        static void Loop() 
        { 
        };
        static void End() 
        { 
        };
    private:
        LoRa() : RTOS::ActiveObject<LoRa>() { };
    };

}

/**
* Build the static members on the RTOS::ActiveObject
*/
namespace Service {    
    template<>
    std::string RTOS::ActiveObject<LoRa>::mName = std::string("LoRa");
    template<>
    uint8_t RTOS::ActiveObject<LoRa>::mCountLoops = 0;
}



#endif
