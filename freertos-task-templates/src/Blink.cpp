/*
 * @fcarraustewart
 * esp32 wrover kit tests.
 */

#include <Arduino.h>
#include "LoRa.hpp"
#include "BLE.hpp"

void setup()
{
}

void loop()
{
  delay(5);

  Service::BLE::Run();  
  Service::LoRa::Run();

  delay(40);
}
