#ifndef __SYSTEM__H
#define __SYSTEM__H
#include <variant>
#include <vector>
#include "MsgBroker.hpp"
#include "Services/LoRa.hpp"
#include "Services/BLE.hpp"
#include "Services/HardwareTimers.hpp"
#include "Utils/overload.hpp"

static const uint16_t msg = 0xAAAA;
static const RTOS::MsgBroker::Message msgEE = {
    .mSource = 0,
    .mDestination = 1,
    .mEvent = RTOS::MsgBroker::Event::BLEConnected,
    .mLength = 1,
    .mPayload = {0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE},
};

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

            for (auto &v : mSystemServicesRegistered)
            {

                std::visit(overload{/*
                                    One of these lambdas will be called for each type in mSystemServicesRegistered.
                                    */
                                    [](const Service::BLE &x)
                                    {
                                        Logger::Log("Initializing: %s", x.mName.c_str());
                                        x.Create();
                                    },
                                    [](const Service::LoRa &x)
                                    {
                                        Logger::Log("Initializing: %s", x.mName.c_str());
                                        x.Create();
                                    },
                                    [](const Service::HardwareTimers &x)
                                    {
                                        Logger::Log("Initializing: %s", x.mName.c_str());
                                        x.Create();
                                    }},
                        /*
                            The element of the mSystemServicesRegistered to which the lambda will be applied.
                        */
                        v

                );

            }
            mMsgBroker.Create();
    }
};

std::vector<std::variant<_REGISTERED_SERVICES>> System::mSystemServicesRegistered = {REGISTERED_SERVICES};
RTOS::MsgBroker System::mMsgBroker = RTOS::MsgBroker();

#endif