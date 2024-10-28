/*
 * Copyright (c) 2024 fcarraustewart, SAS.
 *
 */

#include <zephyr/sys/util.h>
namespace Service {
    class BLE {
        public:
        static int init(void);
        static int send(void);
    };
}
