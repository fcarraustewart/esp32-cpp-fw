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
    // FIXME:
    //      Instead of using bool flags here, we can make the activeObjects 
    //  a vector of pointers rather than being objects themselves
    //  and checking for if nullptr == RTOS::ActiveObject<Service::BLE>* 
    //  is much easier and less cumbersome. 
    
    //      In the construction processes we can populate this vector as needed.
    
    //      Another idea is to actually use a registeredServices vector that
    //  gets passed around to every ActiveObject constructed and, when under
    //  construction, every ActiveObject uses this vector to register 
    //  itself onto it. 
    //      This sounds scalable and the best part is that it can 
    //  actually be resolved in the ActiveObject's source code

    //      Also, we can remove the Create() method that only exists to control
    //  execution of rtos tasks. This can pass to the constructor and we can use
    //  a pre-init-common-message to actually START the system after it is constructed, 
    //  and lets all activeObjects run loose. 
    //      The idea is that we block them waiting for pre-init-ready 
    //  at the beginning of their rtos task.
    
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

void RTOS::MsgBroker::PostEvent(const Event& event)
{
    // TODO: Try to create a Dispatch table with REGISTERED_SERVICES at compile time.
    switch(event)
    {
        Logger::Log("Event Post: %d", event);
        case Event::BLEConnected:
        {
            // Dispatch to every service suscribed to BLE STATE notifications
            /*
             Kind of https://stackoverflow.com/questions/48017/what-is-a-jump-table
             Assembly style:
                    distribute:
                        ld r2, event & suscribedServicesIDs; // or rotate, somekind of math multiplexing
                        andi r2, suscribedServicesIDs; 
                        jp r2; // jumptable
                        jp sendToBLEService;
                        jp distribute;
                        jp distribute;
                        jp distribute;
                        jp distribute; // rather than distribute : NoOperation;
                 C/C++ style:                   https://gamedev.stackexchange.com/questions/38127/pointers-to-member-functions-in-an-event-dispatcher
                    (void*)(array) = {registered_fn_pointers,registered_fn_pointers,...};
                    (void*)array_of_pointers_to_functions[event&suscribedServicesIDs]();
            */
            // Debug
            Logger::Log("Event BLEConnected ocurred. \
            Dispatching messages to each of the suscribed services.");
            break;
        }
        case Event::LoRaConnected:
        {
            break;
        }
        default:
        break;
    }
}