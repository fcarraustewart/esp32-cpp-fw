#ifndef MSGBROKER__H_H
#define MSGBROKER__H_H
#include "Services/BLE.hpp"
#include "Services/LoRa.hpp"
#include "Logger.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <string>
// https://commschamp.github.io/comms_protocols_cpp/ see this for further development
namespace RTOS
{
    class MsgBroker
    {
    public:
        static const    uint16_t            cMaxPayloadLength = 10;
        using           payload_t = uint8_t[cMaxPayloadLength];
        
        enum class Event
        {
            BLEConnected,
            LoRaConnected,
        };
        struct __attribute__ ((packed)) Message
        {
            uint8_t     mSource;
            uint8_t     mDestination;
            uint16_t    mLength;
            payload_t   mPayload;
            Event       mEvent;

            //Message(Event& event, payload_t& p, uint16_t& l){}; Constructor?
        };
    public:
        static void PostEvent(const Event& event);
        //template<typename... TArgs>
        static void Create();
    private:


    protected:

    /**
     *                  Member Variables:
     */
    public:

    protected:
    };

}

#endif








/*

    union example_union {
        struct {
            uint32_t EXAMPLE_FIELD_0 : 8;
            uint32_t EXAMPLE_FIELD_1 : 8;
            uint32_t EXAMPLE_FIELD_2 : 8;
            uint32_t RESERVED : 8;
        } bytes;
        struct {
            uint32_t BIT_0 : 1;
            uint32_t BIT_1 : 1;
            uint32_t BIT_2 : 1;
            uint32_t BIT_3 : 1;
            uint32_t BIT_4 : 1;
            uint32_t BIT_5 : 1;
            uint32_t BIT_6 : 1;
            uint32_t BIT_7 : 1;
            uint32_t BIT_8 : 1; 
            uint32_t BIT_9 : 1; 
            uint32_t BIT_10 : 1; 
            uint32_t BIT_11 : 1; 
            uint32_t BIT_12 : 1; 
            uint32_t BIT_13 : 1; 
            uint32_t BIT_14 : 1; 
            uint32_t BIT_15 : 1;
            uint32_t BIT_16 : 1;
            uint32_t BIT_17 : 1;
            uint32_t BIT_18 : 1;
            uint32_t BIT_19 : 1;
            uint32_t BIT_20 : 1;
            uint32_t BIT_21 : 1;
            uint32_t BIT_22 : 1;
            uint32_t BIT_23 : 1;
            uint32_t RESERVED : 8;
        } bits;
        uint32_t u32All;
    };
  union RTOS::MsgBroker::example_union cosa;

  cosa.u32All = 0;
  Logger::Log("[Service::%s]::%s():\t cosa es %X.", "loop", __func__, cosa.u32All);
  cosa.bits.BIT_0 = 1;
  cosa.bits.BIT_4 = 1;
  cosa.bits.BIT_7  = 1;
  cosa.bits.BIT_23  = 1;
  Logger::Log("[Service::%s]::%s():\t cosa es %X.", "loop", __func__, cosa.u32All);
  cosa.bytes.EXAMPLE_FIELD_1 = 55 ;
  Logger::Log("[Service::%s]::%s():\t cosa es %X.", "loop", __func__, cosa.u32All);



*/