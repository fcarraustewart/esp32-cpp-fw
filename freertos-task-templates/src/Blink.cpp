/*
 * @fcarraustewart
 * esp32 wrover kit tests.
 */

#include <Arduino.h>
#include "Services/LoRa.hpp"
#include "Services/BLE.hpp"

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
