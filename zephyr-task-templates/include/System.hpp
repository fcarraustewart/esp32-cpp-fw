#ifndef __SYSTEM__H
#define __SYSTEM__H
#include <variant>
#include <vector>
#include "Services/LoRa.hpp"
#include "Services/HardwareTimers.hpp"
#include "Utils/overload.hpp"

class System {
#define _REGISTERED_SERVICES    Service::LoRa,  Service::HardwareTimers
#define REGISTERED_SERVICES     Service::LoRa{}, Service::HardwareTimers{}
public:
    static std::vector<std::variant<_REGISTERED_SERVICES>> mSystemServicesRegistered;
public:
    static constexpr void Create()
    {            
            /**
             * @brief
             *
             * Variants templates indicate types that the variable variant can take.
             * Overload indicates the lambda done as operator() to each of the types you are interested in.
             * visit uses the operator() on the correct type that was loaded to the variant at the time visit is used.
             *
             *
             */
            //LOG_INF("mSystemServicesRegistered Address a pointer at = 0x%08x", (unsigned int)&mSystemServicesRegistered);
            // Remember you can do mSystemServicesRegistered.push_back(); for each service here.

            for (auto &v : mSystemServicesRegistered)
            {
                //LOG_INF("Testing Create()");
                std::visit( 
                    overload{
                        /*
                            One of these lambdas 
                            will be called for 
                            each type 
                            in mSystemServicesRegistered.
                        */
                        [](const Service::LoRa &x)
                        {
                            x.Create();
                            // LOG_INF("Initializing: %s", x.mName);
                            // LOG_INF("Address a pointer at = 0x%08x", (unsigned int)&x);
                        },
                        [](const Service::HardwareTimers &x)
                        {
                            // LOG_INF("Initializing: %s", x.mName);
                            // LOG_INF("Address a pointer at = 0x%08x", (unsigned int)&x);
                            x.Create();
                        }},
                v);

            }
    };
};


#endif