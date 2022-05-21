#ifndef MSGBROKER__H_H
#define MSGBROKER__H_H
#include "Services/BLE.hpp"
#include "Services/LoRa.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <string>

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
            Event       mEvent;
            uint16_t    mLength;
            payload_t   mPayload;

            //Message(Event& event, payload_t& p, uint16_t& l){}; Constructor?
        };
    public:
        
        template<typename... TArgs>
        static void Create()
        {
        };
    private:


    protected:

    /**
     *                  Member Variables:
     */
    public:

    protected:
    };

}



RTOS::MsgBroker& operator<<(RTOS::MsgBroker& obj, RTOS::MsgBroker::Message& msg)
{
    bool bleServiceRegistered = true;   //TODO: Use some kind of register process with a template<Args...> MsgBroker::Create()
    switch( msg.mDestination )
    {
        default:
        {
            if(bleServiceRegistered)
                Service::BLE::Send((uint8_t*)   &msg.mEvent     );
            break;
        };
    };        

    return obj;
}
#endif
