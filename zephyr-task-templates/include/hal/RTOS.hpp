#ifndef __RTOS__H_H
#define __RTOS__H_H

#define SystemUsesFreeRTOS 0
#define SystemUsesZephyrRTOS 1

#if SystemUsesFreeRTOS == 1
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <thread>
#include <queue>
#endif

#if SystemUsesZephyrRTOS == 1
#include <zephyr/kernel.h>
#include <Utils/zpp.hpp>

#endif

namespace RTOS 
{
#if SystemUsesZephyrRTOS == 1
    const zpp::thread_attr cThreadAttributes(zpp::thread_prio::preempt(1),
        zpp::thread_inherit_perms::no,
        zpp::thread_suspend::yes
    );
    typedef k_msgq QueueHandle_t   ;
    typedef zpp::thread TaskHandle_t    ;
    typedef k_thread_entry_t TaskFunction_t  ;
#endif
    using   func_t = void (*)() noexcept;
    struct Hal
    {
        static void inline TaskCreate(func_t thread, const uint8_t name[], TaskHandle_t* handle) 
        {
        #if SystemUsesFreeRTOS == 1
            xTaskCreate(&thread, name, 4096, 
                    nullptr, 2, 
                    &handle);
        #endif

        #if SystemUsesZephyrRTOS == 1
        ARG_UNUSED(thread);
        ARG_UNUSED(name);
        
            if(handle->start());
            
        #endif

        };
        static RTOS::QueueHandle_t QueueCreate (uint8_t const bytesLength, uint8_t const bytesSize, char * queueAllocation);
        static bool inline QueueReceive(void * queue, void * receivedMsg)
        // Should be re-entrant: as long as msgq is re-entrant and thread-safe
        {
        #if SystemUsesFreeRTOS == 1
            if(pdTRUE == xQueueReceive((QueueHandle_t)queue, (void*)receivedMsg, portMAX_DELAY))
                return true;
            return false;
        #endif

        #if SystemUsesZephyrRTOS == 1
            if(0 == k_msgq_get((QueueHandle_t*)queue, receivedMsg, K_FOREVER)) {            
                return true;
            }
        #endif
            return false;
        };
        static void QueueSend(void * queue, const uint8_t msg[]);
    };
}
#endif