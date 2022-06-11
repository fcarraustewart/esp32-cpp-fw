#include "System.hpp"

static bool bleServiceRegistered = true;   //TODO: Use some kind of register process with a template<Args...> MsgBroker::Create()
    
void RTOS::MsgBroker::Create()
{
    // try
    // {
    //     // The parameter of at(i) should be i = 1 because that's the position where we put LoRa in the variant.
    //     std::vector<std::variant<_REGISTERED_SERVICES>>::iterator servicesIterator;
    //     for(servicesIterator = System::mSystemServicesRegistered.begin(); 
    //         servicesIterator != System::mSystemServicesRegistered.end() ; 
    //         servicesIterator++)
    //     {
    //         auto x = std::get<Service::BLE>(*servicesIterator);
    //         bleServiceRegistered = true;
    //     }
    // }
    // catch (std::bad_variant_access const& ex)
    // {
    //     Logger::Log("Bad Variant Access -> %s.", ex.what());
    // }
    

};

RTOS::MsgBroker& operator<<(RTOS::MsgBroker& obj, RTOS::MsgBroker::Message& msg)
{

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