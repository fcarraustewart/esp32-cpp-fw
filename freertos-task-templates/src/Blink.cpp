/*
 * @fcarraustewart
 * esp32 wrover kit tests.
 */

#include <Arduino.h>
#include "System.hpp"
#include "Logger.hpp"
#include <string>


void setup()
{
  System::Create();
}

void loop()
{
  delay(400);
  Service::BLE::Send((uint8_t *)&msg);
  Service::LoRa::Send((uint8_t *)&msg);
  delay(400);
}
