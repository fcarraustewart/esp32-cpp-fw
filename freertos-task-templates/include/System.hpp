#ifndef __SYSTEM__H
#define __SYSTEM__H
#include <variant>
#include <vector>
#include "MsgBroker.hpp"
#include "Services/LoRa.hpp"
#include "Services/BLE.hpp"
#include "Services/HardwareTimers.hpp"
#include "Utils/overload.hpp"


class System {
#define _REGISTERED_SERVICES    Service::BLE,   Service::LoRa,  Service::HardwareTimers
#define REGISTERED_SERVICES     Service::BLE{}, Service::LoRa{}, Service::HardwareTimers{}
public:
    static RTOS::MsgBroker mMsgBroker;
    static std::vector<std::variant<_REGISTERED_SERVICES>> mSystemServicesRegistered;
public:
    static void Create()
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
            Logger::Log("mSystemServicesRegistered Address a pointer at = 0x%08x", &mSystemServicesRegistered);

            for (auto &v : mSystemServicesRegistered)
            {
                Logger::Log("v Address a pointer at = 0x%08x", &v);

                std::visit( overload{/*
                                    One of these lambdas will be called for each type in mSystemServicesRegistered.
                                    */
                                    [](const Service::BLE &x)
                                    {
                                        Logger::Log("Initializing: %s", x.mName.c_str());
                                        Logger::Log("Address a pointer at = 0x%08x", &x);
                                        x.Create();
                                    },
                                    [](const Service::LoRa &x)
                                    {
                                        Logger::Log("Initializing: %s", x.mName.c_str());
                                        Logger::Log("Address a pointer at = 0x%08x", &x);
                                        x.Create();
                                    },
                                    [](const Service::HardwareTimers &x)
                                    {
                                        Logger::Log("Initializing: %s", x.mName.c_str());
                                        Logger::Log("Address at = 0x%08x", &x);
                                        x.Create();
                                    }},
                            /*
                                The element of the mSystemServicesRegistered to which the lambda will be applied.
                            */
                            v);

            }
            /*
                Msg
            */
            mMsgBroker.Create();
    }
};


#endif