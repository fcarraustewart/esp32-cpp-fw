/*
 * @fcarraustewart
 * esp32 wrover kit tests.
 */


#include <Arduino.h>
#include "Services/LoRa.hpp"
#include "Services/BLE.hpp"
#include "MsgBroker.hpp"

static RTOS::MsgBroker M = RTOS::MsgBroker();

static volatile uint16_t interruptCounter;
static hw_timer_t * timer = NULL;
static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
static const uint16_t msg = 0xAAAA;
static const uint8_t msgEE[] = {0xEE};

void IRAM_ATTR onTimer() {
  Service::BLE::Send((uint8_t*)&(++interruptCounter));  /**< This runs the xQueueFromISR version of Send. */
  M << msgEE;
}
void setup() {
  timer = timerBegin(0, 2, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
  Service::BLE::Create();
  Service::LoRa::Create();
}

void loop()
{
  delay(400);
  Service::BLE::Send((uint8_t*)&msg);  
  Service::LoRa::Send((uint8_t*)&msg);  
  delay(400);
}
 