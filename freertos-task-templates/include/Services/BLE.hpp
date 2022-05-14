#ifndef SERVICE_BLE__H_H
#define SERVICE_BLE__H_H
#include "FreeRTOS.h"
#include "ActiveObject.hpp"
/**
* Customize the static methods of an ActiveObject
*/
namespace Service{
    class BLE : public ActiveObject<BLE>
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
        BLE() : ActiveObject<BLE>() { };
    };

}
/**
* Build the static members on the ActiveObject
*/
namespace Service {    
    template<>
    std::string ActiveObject<BLE>::mName = std::string("BLE");
    template<>
    uint8_t ActiveObject<BLE>::mCountLoops = 0;
}



#endif