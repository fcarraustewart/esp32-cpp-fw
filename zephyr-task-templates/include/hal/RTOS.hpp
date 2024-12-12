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
#endif


// #if SystemUsesFreeRTOS == 1
// typedef RTOS::QueueHandle_t QueueHandle_t;
// typedef RTOS::TaskHandle_t  TaskHandle_t;
// namespace RTOS 
// {
//     class FreeRTOSImpl : public RTOS::Hal<FreeRTOSImpl>
//     {
//         static void TaskCreate const(void * arg){
//             xTaskCreate(&Run, mName, STACKSIZE, 
//                     nullptr, 2, 
//                     &mHandle);
//         };
//         static RTOS::QueueHandle_t QueueCreate QueueCreate const(uint8_t const bytesLength, uint8_t const bytesSize, void * queueAllocation){
//             RTOS::QueueHandle_t createdQueue;
//             createdQueue = xQueueCreate(bytesLength, bytesSize);
//             return createdQueue;
//         };
//         static void QueueSend (void * queue, const uint8_t msg[]){
//             if( xPortInIsrContext() )                               /**< Does this work in another platform rather than ESP32? */
//             {    
//                 BaseType_t xHigherPriorityTaskWoken;
//                 xQueueSendFromISR((QueueHandle_t)queue,  (void*) msg, &xHigherPriorityTaskWoken);
//                 if(xHigherPriorityTaskWoken)
//                     portYIELD_FROM_ISR();
//             } 
//             else
//             {
//                 xQueueSend((QueueHandle_t)queue, (void*) msg, 0);
//             }
//         };
//         static bool QueueReceive const(void * queue, void * receivedMsg){
//             if(pdTRUE == xQueueReceive((QueueHandle_t)queue, (void*)receivedMsg, portMAX_DELAY))
//                 return true;
//             return false;
//         };
//     };
// }
// #endif

// #if SystemUsesZephyrRTOS == 1

// using QueueHandle_t   =  k_msgq;
// using TaskHandle_t    =  k_thread;
// using TaskFunction_t  =  k_thread_entry_t;
// class ZephyrImpl : public RTOS::Hal<ZephyrImpl>
// {
//     static void TaskCreate (void * arg){
//         struct k_thread mHandle;
//         K_THREAD_STACK_DEFINE(mMemoryAllocationStack, STACKSIZE);
//         k_thread_create(&mHandle, mMemoryAllocationStack, STACKSIZE,
//                 (k_thread_entry_t) Run,
//                 NULL, NULL, NULL, K_PRIO_COOP(7), 0, K_NO_WAIT);
//     };
//     static RTOS::QueueHandle_t QueueCreate QueueCreate const(uint8_t const bytesLength, uint8_t const bytesSize, void * queueAllocation){
//         RTOS::QueueHandle_t createdQueue;
//         k_msgq_init(&createdQueue, queueAllocation, bytesLength, bytesSize);
//         return createdQueue;
//     };
//     static void QueueSend (void * queue, const uint8_t msg[]){
//         while (k_msgq_put(&queue, &msg, K_NO_WAIT) != 0) {   /**< Send data to consumers */
//             k_msgq_purge(&queue);			                    /**< Message queue is full: purge old data & try again */
//         }
//     };
//     static bool QueueReceive (void * queue, void * receivedMsg){
//         k_msgq_get(&queue, &receivedMsg, K_FOREVER);            
//         return true;
//     };
// };


// #endif

namespace RTOS 
{
    typedef k_msgq QueueHandle_t   ;
    typedef k_thread TaskHandle_t    ;
    typedef k_thread_entry_t TaskFunction_t  ;
    using func_t = void (*)() noexcept;
    // template <class Impl>
    // class Hal
    // {
    //     static void                 TaskCreate const(void * arg){Impl::TaskCreate(arg);};
    //     static RTOS::QueueHandle_t  QueueCreate const(uint8_t const bytesLength, uint8_t const bytesSize, void * queueAllocation){return Impl::QueueCreate(bytesLength, bytesSize, queueAllocation);};
    //     static void                 QueueSend const(void * queue, const uint8_t msg[]){Impl::QueueSend(queue, msg);};
    //     static bool                 QueueReceive const(void * queue, void * receivedMsg){return Impl::QueueReceive(queue, receivedMsg);};
    // };
    struct Hal
    {
        static void TaskCreate(func_t thread, const uint8_t name[], TaskHandle_t* handle);
        static RTOS::QueueHandle_t QueueCreate (uint8_t const bytesLength, uint8_t const bytesSize, char * queueAllocation);
        static bool QueueReceive(void * queue, void * receivedMsg);
        static void QueueSend(void * queue, const uint8_t msg[]);
        static uint8_t mThreadCount; 
    };
}
#endif