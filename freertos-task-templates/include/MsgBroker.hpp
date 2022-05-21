#ifndef MSGBROKER__H_H
#define MSGBROKER__H_H
#include "Services/BLE.hpp"
#include "Services/LoRa.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <string>

using packet_t = uint8_t;

namespace RTOS
{
    class MsgBroker
    {
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



RTOS::MsgBroker& operator<<(RTOS::MsgBroker& m, const uint8_t msg[])
{
    bool bleServiceRegistered = true;   //TODO: Use some kind of register process with a template<Args...> MsgBroker::Create()
    switch(msg[0])
    {
        default:
        {
            if(bleServiceRegistered)
                Service::BLE::Send(msg);
            break;
        };
    };        

    return m;
}
#endif
