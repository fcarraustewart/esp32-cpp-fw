/*
 * @fcarraustewart
 * esp32 wrover kit tests.
 */


#include <Arduino.h>
#include "Services/LoRa.hpp"
#include "Services/BLE.hpp"
#include "MsgBroker.hpp"
#include "Logger.hpp"
#include <string>
#include <variant>
#include <vector>

static RTOS::MsgBroker M = RTOS::MsgBroker();
static volatile uint16_t interruptCounter;
static hw_timer_t * timer = NULL;

static const uint16_t msg = 0xAAAA;
static const RTOS::MsgBroker::Message msgEE = {
      .mSource =        0,
      .mDestination =   1,
      .mEvent =         RTOS::MsgBroker::Event::BLEConnected,
      .mLength =        1,
      .mPayload =       {0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE},
};

void IRAM_ATTR onTimer() {
  Service::BLE::Send((uint8_t*)&(++interruptCounter));  /**< This runs the xQueueFromISR version of Send. */
  //M << msgEE;
}

template<class... Ts> 
struct overload : Ts... 
{ 
  using Ts::operator()...; 
};
template<class... Ts> 
overload(Ts...) -> overload<Ts...>;

void CreationOfVariantVectorWithServices()
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
    std::vector<  std::variant<   Service::BLE,   Service::LoRa   >>      // 1
                  vecVariant = {  Service::BLE{}, Service::LoRa{} };
  
    for (auto& v: vecVariant){
      
        std::visit( overload  { 
                      /* 
                        One of these lambdas will be called for each type in vecVariant. 
                      */
                      [](const Service::BLE& x)         { Logger::Log(  "Initted: %s",      x.mName.c_str());   x.Create();  },
                      [](const Service::LoRa& x)        { Logger::Log(  "Initted: %s",      x.mName.c_str());   x.Create(); }
                    }, 
                      /* 
                        The element of the vecVariant to which the lambda will be applied. 
                      */
                    v 
                  
                  ); 

    }
  
}

void setup() {
  CreationOfVariantVectorWithServices();
  timer = timerBegin(0, 2, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
}


void test()
{
    std::variant<int, float, std::string> v1 { "Hello" };
    std::variant<int, float, std::string> v2 { 1 };
    std::variant<int, float, std::string> v3 { 20.0f };
    std::visit( overload  {
                  [](const int& i)          { Logger::Log("int: %d", i);     },
                  [](const float& f)        { Logger::Log("float: %f", f);   },
                  [](const std::string& s)  { Logger::Log("string: %s", s.c_str());  }
                }, v1 ); 
    std::visit( overload  {
                  [](const int& i)          { Logger::Log("int: %d", i);     },
                  [](const float& f)        { Logger::Log("float: %f", f);   },
                  [](const std::string& s)  { Logger::Log("string: %s", s);  }
                }, v2 ); 
    std::visit( overload  {
                  [](const int& i)          { Logger::Log("int: %d", i);     },
                  [](const float& f)        { Logger::Log("float: %f", f);   },
                  [](const std::string& s)  { Logger::Log("string: %s", s);  }
                }, v3 ); 
}
             
void loop()
{
  delay(400);
  Service::BLE::Send((uint8_t*)&msg);  
  Service::LoRa::Send((uint8_t*)&msg);  
  test();
  delay(400);
}
 