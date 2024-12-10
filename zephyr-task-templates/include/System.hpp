#ifndef __SYSTEM__H
#define __SYSTEM__H
#include <variant>
#include <vector>
#include "Services/LoRa.hpp"
#include "Services/HardwareTimers.hpp"
#include "Utils/overload.hpp"

class System {
#define _REGISTERED_SERVICES    Service::LoRa//,  Service::HardwareTimers
#define REGISTERED_SERVICES     Service::LoRa{}//, Service::HardwareTimers{}
public:
    static std::vector<std::variant<_REGISTERED_SERVICES>> mSystemServicesRegistered;
public:
    static void Create();
};


#endif