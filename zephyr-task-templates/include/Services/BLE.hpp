/*
 * Copyright (c) 2024 fcarraustewart, SAS.
 *
 */
#ifndef _________SERVICE_BLE_H____________
#define _________SERVICE_BLE_H____________

#include <zephyr/sys/util.h>
namespace Service {
    class BLE {
        public:
        static int init(void);
        static int send(char* arg);
    };
}

#endif //_________SERVICE_BLE_H____________
