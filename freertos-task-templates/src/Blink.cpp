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
static const uint16_t msg = 0xAAAA;
void loop()
{
  delay(5);

  Service::BLE::Send((uint8_t*)&msg);  

  Service::BLE::Run();  

  delay(40);
}
