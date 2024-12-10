# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(cpp_synchronization)

include_directories(include)

set(CMAKE_CXX_STANDARD 17)

target_sources(app PRIVATE 
src/main.cpp 
src/System.cpp
src/hal/RTOS.cpp
src/Services/BLE.cpp 
src/Services/LEDs.cpp 
src/Services/Sensor.cpp 
src/Services/IMU.cpp 
src/Services/LoRa.cpp
src/Services/HardwareTimers.cpp
)
