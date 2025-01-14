#include <System.hpp>

// Check if needed. May be removed.
// TODO: Check if the constructors are being prbly called without constexpr.
std::vector<std::variant<_REGISTERED_SERVICES>> 
    System::mSystemServicesRegistered = {
        REGISTERED_SERVICES
    };
