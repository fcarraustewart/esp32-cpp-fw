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

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

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
static  uint8_t one = 1;
static  uint8_t other = 0xA5;
static  double flo = 3.14f;
void setup() {
  timer = timerBegin(0, 2, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
  Service::BLE::Create();
  Service::LoRa::Create();
  Logger::Log( "[%s] WARNING!!! LOG TEST #%d. Other = 0x%x, flo = %f.", __func__, one, other, flo ); /**< outputs: [setup] WARNING!!! LOG TEST #1. Other = 0xa5, flo = 3.140000.*/
}

void test()
{
    std::variant<int, float, std::string> intFloatString { "Hello" };
    std::visit(overload  {
        [](const int& i) {          std::cout << "int: " << i; },
        [](const float& f) {        std::cout << "float: " << f; },
        [](const std::string& s) {  std::cout << "string: " << s; }
      },
      intFloatString
    ); 
}
           
void loop()
{
  delay(400);
  Service::BLE::Send((uint8_t*)&msg);  
  Service::LoRa::Send((uint8_t*)&msg);  
  delay(400);
}
 