#include "System.hpp"

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(sys);

void System::Create()
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
        LOG_INF("mSystemServicesRegistered Address a pointer at = 0x%08x", (unsigned int)&mSystemServicesRegistered);
        // Remember you can do mSystemServicesRegistered.push_back(); for each service here.

        for (auto &v : mSystemServicesRegistered)
        {
            LOG_INF("Testing Create()");
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
                        LOG_INF("Initializing: %s", x.mName);
                        LOG_INF("Address a pointer at = 0x%08x", &x);
                    },
                    [](const Service::HardwareTimers &x)
                    {
                        LOG_INF("Initializing: %s", x.mName);
                        LOG_INF("Address a pointer at = 0x%08x", &x);
                        x.Create();
                    }},
            v);

        }
}

std::vector<std::variant<_REGISTERED_SERVICES>> System::mSystemServicesRegistered = {REGISTERED_SERVICES};
