#ifndef __SYSTEM__H
#define __SYSTEM__H
#include <variant>
#include <vector>
#include <Services/LoRa.hpp>
#include <Services/HardwareTimers.hpp>
#include <Services/BLE.hpp>
#include <Services/LEDs.hpp>
#include <Services/Sensor.hpp>
#include <Services/IMU.hpp>

#include <Utils/overload.hpp>

class System {
#define _REGISTERED_SERVICES    std::monostate, Service::LoRa,  Service::HardwareTimers,    Service::BLE,   Service::LEDs,   Service::Sensor,       Service::IMU
#define REGISTERED_SERVICES     Service::LoRa{}, Service::HardwareTimers{}, Service::BLE{}, Service::LEDs{},   Service::Sensor{},   Service::IMU{}
public:
    static std::vector<std::variant<_REGISTERED_SERVICES>> mSystemServicesRegistered;
public:
	constexpr System() {};	
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
            //using my_variant = std::variant<_REGISTERED_SERVICES>;
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
                        [](const std::monostate &x)
                        {
                            // LOG_INF("Initializing: %s", x.mName);
                            // LOG_INF("Address a pointer at = 0x%08x", (unsigned int)&x);
                        },
                        [](const Service::BLE &x)
                        {
                            if(false == true)
								while(1){RTOS::Hal::Delay(1000);};
                            // LOG_INF("Initializing: %s", x.mName);
                            // LOG_INF("Address a pointer at = 0x%08x", (unsigned int)&x);
                        },
                        [](const Service::IMU &x)
                        {
                            if(false == true)
								while(1){RTOS::Hal::Delay(1000);};
                            // LOG_INF("Initializing: %s", x.mName);
                            // LOG_INF("Address a pointer at = 0x%08x", (unsigned int)&x);
                        },
                        [](const Service::LEDs &x)
                        {
                            if(false == true)
								while(1){RTOS::Hal::Delay(1000);};
                            // LOG_INF("Initializing: %s", x.mName);
                            // LOG_INF("Address a pointer at = 0x%08x", (unsigned int)&x);
                        },
                        [](const Service::Sensor &x)
                        {
                            if(false == true)
								while(1){RTOS::Hal::Delay(1000);};
                            // LOG_INF("Initializing: %s", x.mName);
                            // LOG_INF("Address a pointer at = 0x%08x", (unsigned int)&x);
                        },
                        [](const Service::LoRa &x)
                        {
                            if(false == true)
								while(1){RTOS::Hal::Delay(1000);};
                            // LOG_INF("Initializing: %s", x.mName);
                            // LOG_INF("Address a pointer at = 0x%08x", (unsigned int)&x);
                        },
                        [](const Service::HardwareTimers &x)
                        {
                            if(false == true)
								while(1){RTOS::Hal::Delay(1000);};
                            // LOG_INF("Initializing: %s", x.mName);
                            // LOG_INF("Address a pointer at = 0x%08x", (unsigned int)&x);
                        }
                        
                    },
                v);



                // std::visit([](auto &&arg) -> my_variant { 
                //     using T = std::decay_t<decltype(arg)>;
                //     if constexpr (std::is_same_v<std::monostate, T>) {
                //         return; // arg is std::monostate here
                //     }
                //     else if constexpr (std::is_same_v<Service::LoRa, T>) {
                //         return; // arg is A here
                //     }
                //     else if constexpr (std::is_same_v<Service::BLE, T>) {
                //         return; // arg is B here
                //     }
                //     else if constexpr (std::is_same_v<Service::HardwareTimers, T>) {
                //         return; // arg is B here
                //     }
                //     else if constexpr (std::is_same_v<Service::IMU, T>) {
                //         return; // arg is B here
                //     }
                //     else if constexpr (std::is_same_v<Service::LEDs, T>) {
                //         return; // arg is B here
                //     }
                //     else if constexpr (std::is_same_v<Service::Sensor, T>) {
                //         return; // arg is B here
                //     }
                // }, v);

            }
    };
};


#endif