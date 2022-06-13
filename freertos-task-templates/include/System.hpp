#ifndef __SYSTEM__H
#define __SYSTEM__H
#include <variant>
#include <vector>
#include "MsgBroker.hpp"
#include "Services/LoRa.hpp"
#include "Services/BLE.hpp"
#include "Services/HardwareTimers.hpp"
#include "Services/LEDs.hpp"
#include "Utils/overload.hpp"


class System {
#define _REGISTERED_SERVICES    Service::BLE,   Service::LoRa,  Service::HardwareTimers, Service::LEDs
#define REGISTERED_SERVICES     Service::BLE{}, Service::LoRa{}, Service::HardwareTimers{}, Service::LEDs{}
public:
    static RTOS::MsgBroker mMsgBroker;
    static std::vector<std::variant<_REGISTERED_SERVICES>> mSystemServicesRegistered;
public:
    static void Create();
};


#endif