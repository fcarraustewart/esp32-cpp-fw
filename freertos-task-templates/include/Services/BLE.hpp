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
        static void Handle(const uint8_t arg[]){
            /**
             * Handle arg packet.
             */
            switch(arg[0])
            {
                case 5:
                {
                    printf("[Service::%s\t]::Handles:\t%x. Pass to LoRa.\r\n", mName.c_str(), arg[0]);
                    Service::LoRa::Send(arg);
                    break;
                }
                default:
                {
                    printf("[Service::%s\t]::Handles:\t%x.\tNYI.\r\n", mName.c_str(), arg[0]);
                    break;
                }
            };
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
    using               _BLE = RTOS::ActiveObject<Service::BLE>;

    template <>
    const std::string   _BLE::mName = std::string("BLE");
    template <>
    uint8_t             _BLE::mCountLoops = 0;
    template <>
    const uint8_t       _BLE::mInputQueueItemLength = 16;
    template <>
    const uint8_t       _BLE::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t        _BLE::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::BLE>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::BLE>::mInputQueueItemSize;
    template <>
    uint8_t             _BLE::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::BLE>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    QueueHandle_t       _BLE::mInputQueue = xQueueCreate(
                                        RTOS::ActiveObject<Service::BLE>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::BLE>::mInputQueueItemSize
                                    );
    template <>
    TaskHandle_t        _BLE::mHandle = 0;
    template <>
    uint8_t             _BLE::mReceivedMsg[
                                        RTOS::ActiveObject<Service::BLE>::mInputQueueItemSize
                                    ] = { 0 };
}

#endif
