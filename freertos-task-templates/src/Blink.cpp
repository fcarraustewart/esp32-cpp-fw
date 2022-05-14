/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>
#include "Task.hpp"
// Set LED_BUILTIN if it is not defined by Arduino framework
//#define LED_BUILTIN 2

static const std::string name1 = std::string("HelloWorld");
static Task HelloWorld(name1);
static const std::string name2 = std::string("Task1");
static Task Task1(name2);


static Base<Task> BaseTask1 = Base<Task>(&Task1);
static Base<Task> BaseTask2 = Base<Task>(&HelloWorld);


void setup()
{
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);

  BaseTask1.PrintName();
  BaseTask2.PrintName();

  delay(500);
}
