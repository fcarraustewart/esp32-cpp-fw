#include "System.hpp"
#include <exception>

static uint8_t messageForLEDsService[2]={ADD_TO_BLINK_COLOR_OPCODE, 0x00};
void Service::HardwareTimers::Handle(const uint8_t arg[]){
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
        case 5:
        {

            try
            {
                // The parameter of at(i) should be i = 1 because that's the position where we put LoRa in the variant.
                auto x = std::get<Service::LEDs>(System::mSystemServicesRegistered.at(3));
                messageForLEDsService[0]=RESET_BLINK_COLOR_OPCODE;
                Logger::Log("[Service::%s]::%s():\t%x. Pass to LEDs.", mName.c_str(), __func__, arg[0]);
                x.Send(messageForLEDsService);
            }
            catch (std::bad_variant_access const& ex)
            {
                Logger::Log("Bad Variant Access -> %s.", ex.what());
            }
            try
            {
                // The parameter of at(i) should be i = 1 because that's the position where we put LoRa in the variant.
                auto x = std::get<Service::BLE>(System::mSystemServicesRegistered.at(0));
                Logger::Log("[Service::%s]::%s():\t%x. Pass to BLE.", mName.c_str(), __func__, arg[0]);
                x.Send(arg);
            }
            catch (std::bad_variant_access const& ex)
            {
                Logger::Log("Bad Variant Access -> %s.", ex.what());
            }
            
            break;
        }
        default:
        {
            Logger::Log("[Service::%s]::%s():\t%x.\tNYI.", mName.c_str(), __func__, arg[0]);
            break;
        }
    };
}



/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using               _HardwareTimers = RTOS::ActiveObject<Service::HardwareTimers>;

    template <>
    const std::string   _HardwareTimers::mName = std::string("HardwareTimers");
    template <>
    uint8_t             _HardwareTimers::mCountLoops = 0;
    template <>
    const uint8_t       _HardwareTimers::mInputQueueItemLength = 16;
    template <>
    const uint8_t       _HardwareTimers::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t        _HardwareTimers::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize;
    template <>
    uint8_t             _HardwareTimers::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    QueueHandle_t       _HardwareTimers::mInputQueue = xQueueCreate(
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize
                                    );
    template <>
    TaskHandle_t        _HardwareTimers::mHandle = 0;
    template <>
    uint8_t             _HardwareTimers::mReceivedMsg[
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize
                                    ] = { 0 };
}

/** 
 * Build specific Service static members
 *  Otherwise it complains that the RTOS 
 * activeObject does not have these members, which is true.
*/
namespace Service
{
    volatile uint16_t   HardwareTimers::mInterruptCounter = 0;
    hw_timer_t*         HardwareTimers::mTimer = nullptr;
}
