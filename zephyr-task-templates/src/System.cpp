#include "System.hpp"

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
        //LOG_INF("mSystemServicesRegistered Address a pointer at = 0x%08x", (unsigned int)&mSystemServicesRegistered);
        // Remember you can do mSystemServicesRegistered.push_back(); for each service here.

        for (auto &v : mSystemServicesRegistered)
        {
            std::visit( overload{/*
                                One of these lambdas will be called for each type in mSystemServicesRegistered.
                                */
                                [](const std::variant<_REGISTERED_SERVICES> &x)
                                {
                                    //try     
                                     
                                        auto service = std::get<Service::LoRa>(x);
                                        //LOG_INF("Initializing: %s", service.mName);
                                        service.Create();
                                    
                                    //catch   (std::bad_variant_access const& ex){}
                                    //try     
                                     
                                        // auto service = std::get<Service::HardwareTimers>(x);  
                                        // service.Create();
                                        //LOG_INF("Initializing: %s", service.mName);
                                    
                                    //catch   (std::bad_variant_access const& ex){}
                                    
                                                         
                                }},
                        /*
                            The element of the mSystemServicesRegistered to which the lambda will be applied.
                        */
                        v);

        }
}

std::vector<std::variant<_REGISTERED_SERVICES>> System::mSystemServicesRegistered = {REGISTERED_SERVICES};
