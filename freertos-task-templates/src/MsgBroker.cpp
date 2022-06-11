#include "System.hpp"

static bool bleServiceRegistered = false;   //TODO: Use some kind of register process with a template<Args...> MsgBroker::Create()
static bool loraServiceRegistered = false;   //TODO: Use some kind of register process with a template<Args...> MsgBroker::Create()
static bool hwTimersServiceRegistered = false;   //TODO: Use some kind of register process with a template<Args...> MsgBroker::Create()

void RTOS::MsgBroker::Create()
{
    // The parameter of at(i) should be i = 1 because that's the position where we put LoRa in the variant.
    std::vector<std::variant<_REGISTERED_SERVICES>>::iterator servicesIterator;
    for(servicesIterator = System::mSystemServicesRegistered.begin(); 
        servicesIterator != System::mSystemServicesRegistered.end() ; 
        servicesIterator++)
    {
        try{
            auto x = std::get<Service::BLE>(*servicesIterator);
            bleServiceRegistered = true;
        }
        catch (std::bad_variant_access const& ex)
        {
        }
        try{
            auto x = std::get<Service::LoRa>(*servicesIterator);
            loraServiceRegistered = true;
        }
        catch (std::bad_variant_access const& ex)
        {
        }
        try{
            auto x = std::get<Service::HardwareTimers>(*servicesIterator);
            hwTimersServiceRegistered = true;
        }
        catch (std::bad_variant_access const& ex)
        {
        }
    }
    
    if(bleServiceRegistered)
        Logger::Log("MsgBroker registers Service::BLE");
    if(loraServiceRegistered)
        Logger::Log("MsgBroker registers Service::LoRa");
    if(hwTimersServiceRegistered)
        Logger::Log("MsgBroker registers Service::HwTimers");
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