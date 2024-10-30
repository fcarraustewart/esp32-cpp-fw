# esp32-cpp-tests
Tests using static templates and lambdas running on Esp32. On the freertos-task-templates note that: No virtual functions are used.

Tests done with platformio under arduino-framework. Or zephyr under esp-idf framework.
Espressif esp-idf should work with platformio just as fine, but arduino has been much faster for the development cycle, as it grabs
 an already compiled version of the esp idf sdk components.

QueueCreateStatic didn't work for this setup but it should for esp idf.

See include/activeobject.hpp this is the templated class that should be inherited by every concrete activeobject: (still wip)

For example:
BLE.hpp specializes and customizes ActiveObject.hpp. no virtual functions were used here.
