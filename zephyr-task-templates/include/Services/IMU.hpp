#ifndef __IMU_h
#define __IMU_h
#include <stdint.h>
#define IMU_ACCEL_ADDRESS (0x4a)
#define IMU_MAG_ADDRESS (0x4a  )

int IMU_begin();
int IMU_readRotXYZ();
int IMU_readGyroXYZ();
int IMU_readMagXYZ();
int IMU_readAccelXYZ();
int IMU_countSteps(volatile uint32_t * pCount);
#endif