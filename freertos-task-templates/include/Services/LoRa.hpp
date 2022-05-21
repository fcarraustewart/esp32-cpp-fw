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
        static void Handle(const uint8_t arg[]){
            /**
             * Handle arg packet.
             */
            switch(arg[0])
            {
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
        LoRa() : RTOS::ActiveObject<LoRa>(){};
    };

}
/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using               _LoRa = RTOS::ActiveObject<Service::LoRa>;

    template <>
    const std::string   _LoRa::mName = std::string("LoRa");
    template <>
    uint8_t             _LoRa::mCountLoops = 0;
    template <>
    const uint8_t       _LoRa::mInputQueueItemLength = 16;
    template <>
    const uint8_t       _LoRa::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t        _LoRa::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::LoRa>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::LoRa>::mInputQueueItemSize;
    template <>
    uint8_t             _LoRa::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::LoRa>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    QueueHandle_t       _LoRa::mInputQueue = xQueueCreate(
                                        RTOS::ActiveObject<Service::LoRa>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::LoRa>::mInputQueueItemSize
                                    );
    template <>
    TaskHandle_t        _LoRa::mHandle = 0;
    template <>
    uint8_t             _LoRa::mReceivedMsg[
                                        RTOS::ActiveObject<Service::LoRa>::mInputQueueItemSize
                                    ] = { 0 };
}

#endif
