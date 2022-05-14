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
        static void Loop(){
            printf("%s InputQueueContents are: 0x", mName.c_str());
            for (size_t i = 0; i < mInputQueueSizeBytes; i++)
            {
                printf("%x", mInputQueueAllocation[i]);
            }
            printf("\r\n");
            printf("%s mInputQueueSizeBytes is: %d\r\n", mName.c_str(), mInputQueueSizeBytes);
            
        };
        static void End(){
            static size_t count = 0;
            if(mCountLoops % 5 == 0){
                mInputQueueAllocation[count] = 0x00;
                mInputQueueAllocation[count+1] = 0xF;  
                count++;
                count = count % mInputQueueSizeBytes;  
            }
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
    const size_t RTOS::ActiveObject<BLE>::mInputQueueSizeBytes = RTOS::ActiveObject<Service::BLE>::mInputQueueItemLength * RTOS::ActiveObject<Service::BLE>::mInputQueueItemSize;

    template <>
    uint8_t RTOS::ActiveObject<BLE>::mInputQueueAllocation[RTOS::ActiveObject<Service::BLE>::mInputQueueSizeBytes] = { 0 };
}

#endif
