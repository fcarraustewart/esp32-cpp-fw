/*
 * @fcarraustewart
 * esp32 wrover kit tests.
 */

#include <Arduino.h>
#include "System.hpp"
#include "Logger.hpp"
#include <string>

static volatile uint16_t interruptCounter;
static hw_timer_t *timer = NULL;

void IRAM_ATTR onTimer()
{
  Service::BLE::Send((uint8_t *)&(++interruptCounter)); /**< This runs the xQueueFromISR version of Send. */
  // M << msgEE;
}

void setup()
{
  System::Create();
  timer = timerBegin(0, 2, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
}

void loop()
{
  delay(400);
  Service::BLE::Send((uint8_t *)&msg);
  Service::LoRa::Send((uint8_t *)&msg);
  delay(400);
}
