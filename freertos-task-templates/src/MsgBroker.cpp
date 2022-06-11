#include "System.hpp"

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