#ifndef TASK__H_H
#define TASK__H_H
#include "FreeRTOS.h"

template <class D>
class Base
{
public:
    Base()
    {
    };
    static void Run() 
    { 
        Enter(); 
        Loop();
        End(); 
    };
    static void Enter() 
    { 
        D::Enter(); 
        printf("%s::Enters.\r\n", mName.c_str());
    };
    static void End() 
    { 
        D::End(); 
        printf("%s::Ends.\r\n", mName.c_str());
    };
    static void Loop() 
    { 
        D::Loop(); 
        printf("%s::Loops %d.\r\n", mName.c_str(), mCountLoops);
        mCountLoops++;
    };
private:
protected:
    static std::string mName;
    static uint8_t mCountLoops;
};

namespace Service{
    class BLE : public Base<BLE>
    {
    public:
        static void Enter() 
        { 
        };
        static void Loop() 
        { 
        };
        static void End() 
        { 
        };
    private:
        BLE() : Base<BLE>() { };
    };

    class LoRa : public Base<LoRa>
    {
    public:
        static void Enter() 
        { 
        };
        static void Loop() 
        { 
        };
        static void End() 
        { 
        };
    private:
        LoRa() : Base<LoRa>() { };
    };

}

namespace Service {    
    template<>
    std::string Base<LoRa>::mName = std::string("LoRa");
    template<>
    uint8_t Base<LoRa>::mCountLoops = 0;
    template<>
    std::string Base<BLE>::mName = std::string("BLE");
    template<>
    uint8_t Base<BLE>::mCountLoops = 0;
}



#endif
