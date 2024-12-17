/*
 * Copyright (c) 2024 fcarraustewart, SAS.
 *
 */
#ifndef _________SERVICE_BLE_H____________
#define _________SERVICE_BLE_H____________
#include "ActiveObject.hpp"

namespace Service {
    class BLE : public RTOS::ActiveObject<BLE> {
        static int InitializeDriver(void);
    public:
        static int send(const void *arg, uint16_t len);
        static void Initialize();
        static void Handle(const uint8_t arg[]);
        static void End(){
        };

        BLE() : RTOS::ActiveObject<BLE>(){};
    private:
    };
}

#endif //_________SERVICE_BLE_H____________
