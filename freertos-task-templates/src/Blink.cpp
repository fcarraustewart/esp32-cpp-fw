/*
 * @fcarraustewart
 * esp32 wrover kit tests.
 */

#include <Arduino.h>
#include "Task.hpp"

static const std::string name1 = std::string("HelloWorld");

static Service SVCBaseTask1 = Service(name1);

void setup()
{
}

void loop()
{
  delay(500);

  SVCBaseTask1.PrintName();

  delay(500);
}
