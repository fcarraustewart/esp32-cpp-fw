#ifndef __RTOS__H_H
#define __RTOS__H_H

#define SystemUsesFreeRTOS 0
#define SystemUsesZephyrRTOS 1

#if SystemUsesFreeRTOS == 1
#include <freertos/FreeRTOS.hpp>
#include <freertos/queue.hpp>
#include <freertos/task.hpp>
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
    using QueueHandle_t 			= 	k_msgq;
    using TaskFunction_t 			= 	k_thread_entry_t;
    using TaskHandle_t 				= 	zpp::thread;
	using BackgroundWorkFn_t 		= 	void (*)(struct k_work *);
    using BackgroundWorkHelper_t 	= 	k_work;
    using BackgroundWorkQueue_t 	= 	k_work_q;
	using BackgroundDelWorkHelper_t = 	k_work_delayable;
	using Delay_t 					= 	k_timeout_t;
#endif
    using   func_t = void (*)() noexcept;
    struct Hal
    {
        static bool inline TaskCreate(func_t thread, const char name[], TaskHandle_t* handle) 
        {
        #if SystemUsesFreeRTOS == 1
            xTaskCreate(&thread, name, 4096, 
                    nullptr, 2, 
                    &handle);
        #endif

        #if SystemUsesZephyrRTOS == 1
        ARG_UNUSED(thread);
        ARG_UNUSED(name);
			return handle->start().has_value();
            
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
		static inline void Delay(uint32_t ms)
		{
			zpp::this_thread::sleep_for(std::chrono::milliseconds(ms));
		};
    };
}
#endif