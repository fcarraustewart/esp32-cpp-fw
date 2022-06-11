/*
 * @fcarraustewart
 * esp32 wrover kit tests.
 */

#include <Arduino.h>
#include "System.hpp"
#include <string>

static const uint16_t msg = 0xAAAA;
// static const RTOS::MsgBroker::Message msgEE = {
//     .mSource = 0,
//     .mDestination = 1,
//     .mEvent = RTOS::MsgBroker::Event::BLEConnected,
//     .mLength = 1,
//     .mPayload = {0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE},
// };

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
