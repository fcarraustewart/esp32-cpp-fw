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
        static void Loop(const uint8_t arg[]){
            /**
             * Handle arg packet.
             */
            printf("Service::%s::Handles: %x\r\n", mName.c_str(), arg[0]);
        };
        static void End(){
        };

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
    const uint8_t RTOS::ActiveObject<BLE>::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t RTOS::ActiveObject<BLE>::mInputQueueSizeBytes = 
                                RTOS::ActiveObject<Service::BLE>::mInputQueueItemLength 
                                * RTOS::ActiveObject<Service::BLE>::mInputQueueItemSize;
    template <>
    uint8_t RTOS::ActiveObject<BLE>::mInputQueueAllocation  [
                                                                RTOS::ActiveObject<Service::BLE>::mInputQueueSizeBytes
                                                            ] = { 0 };
    template <>
    QueueHandle_t RTOS::ActiveObject<BLE>::mInputQueue = xQueueCreate(
                                                            mInputQueueItemLength,
                                                            mInputQueueItemLength
                                                        );
}

#endif
