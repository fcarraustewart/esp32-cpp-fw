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
    const std::string RTOS::ActiveObject<BLE>::mName = std::string("BLE");
    template <>
    uint8_t RTOS::ActiveObject<BLE>::mCountLoops = 0;
    template <>
    const uint8_t RTOS::ActiveObject<BLE>::mInputQueueItemLength = 16;
    template <>
    const uint8_t RTOS::ActiveObject<BLE>::mInputQueueItemSize = sizeof(uint8_t);
    // template <>
    // const uint8_t RTOS::ActiveObject<BLE>::mInputQueueAllocation[RTOS::ActiveObject<BLE>::mInputQueueItemLength] = { 0 };
}

#endif
