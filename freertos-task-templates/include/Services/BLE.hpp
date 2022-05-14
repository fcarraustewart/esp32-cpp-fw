#ifndef SERVICE_BLE__H_H
#define SERVICE_BLE__H_H
#include "ActiveObject.hpp"
/**
 * Customize the static methods of an RTOS::ActiveObject
 */
namespace Service
{
    class BLE : public RTOS::ActiveObject<BLE>
    {
    public:
        static void Initialize(){};
        static void Loop(){};
        static void End(){};

    private:
        BLE() : RTOS::ActiveObject<BLE>(){};
    };

}
/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    template <>
    std::string RTOS::ActiveObject<BLE>::mName = std::string("BLE");
    template <>
    uint8_t RTOS::ActiveObject<BLE>::mCountLoops = 0;
}

#endif
