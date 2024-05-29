# esp32-c--tests
Tests using static templates and lambdas running on Esp32

Tests done with platformio under arduino-framework. 
Espressif esp-idf should work just as fine but arduino has been much faster for the development cycle, as it grabs
 an already compiled version of the esp idf sdk components.

QueueCreateStatic didn't work for this setup but it should for esp idf.

See include/activeobject.hpp this is the templated class that should be inherited by every concrete activeobject: (still wip)

https://github.com/fcarraustewart/esp32-c--tests/blob/master/freertos-task-templates/include/ActiveObject.hpp
For example:
BLE.hpp specializes and customizes ActiveObject.hpp. no virtual functions were used here.
