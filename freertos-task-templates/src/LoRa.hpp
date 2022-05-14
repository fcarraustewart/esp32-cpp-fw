#ifndef SERVICE_LORA__H_H
#define SERVICE_LORA__H_H
#include "FreeRTOS.h"
#include "ActiveObject.hpp"

namespace Service{
    class LoRa : public ActiveObject<LoRa>
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
        LoRa() : ActiveObject<LoRa>() { };
    };

}

namespace Service {    
    template<>
    std::string ActiveObject<LoRa>::mName = std::string("LoRa");
    template<>
    uint8_t ActiveObject<LoRa>::mCountLoops = 0;
}



#endif
