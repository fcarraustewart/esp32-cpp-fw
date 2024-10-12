/*
 * @fcarraustewart
 * esp32 wrover kit tests.
 */

#include <Arduino.h>
#include "System.hpp"
#include <string>

static const uint16_t msg = 0xAAAA;

void setup()
{
  System::Create();
}

void loop()
{
  Service::BLE::Send((uint8_t *)&msg);
  Service::LoRa::Send((uint8_t *)&msg);
  delay(1000);
}
