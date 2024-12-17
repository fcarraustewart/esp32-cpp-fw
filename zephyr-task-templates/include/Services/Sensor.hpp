/*
 * Copyright (c) 2024 fcarraustewart, SAS.
 *
 */
#ifndef _________SERVICE_SENSOR_H____________
#define _________SERVICE_SENSOR_H____________
#include <zephyr/sys/util.h>

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>
#include <ActiveObject.hpp>

namespace Service {
    class Sensor : public RTOS::ActiveObject<Sensor>
    {
    public:
        static void Initialize();
        static void Handle(const uint8_t arg[]);
        static void End(){
        };

        Sensor() : RTOS::ActiveObject<Sensor>(){};
    private:
        static int InitializeDriver(void);
        static int send(void);
    };

	static const struct device *const mVCNL = DEVICE_DT_GET_ONE(vishay_vcnl4040);
}

#endif //_________SERVICE_SENSOR_H____________
