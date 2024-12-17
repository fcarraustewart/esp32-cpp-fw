#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(Hal);

#include "hal/RTOS.hpp"

RTOS::QueueHandle_t RTOS::Hal::QueueCreate (uint8_t const bytesLength, uint8_t const bytesSize, char * queueAllocation)
{
    RTOS::QueueHandle_t createdQueue;
#if SystemUsesFreeRTOS == 1
    createdQueue = xQueueCreate(bytesLength, bytesSize);
#endif
#if SystemUsesZephyrRTOS == 1
    k_msgq_init(&createdQueue, queueAllocation, bytesLength, bytesSize);
#endif
    return createdQueue;
}

// Not REentrant: FIXME SHOULD BE ONE FOR EACH CLASS
void RTOS::Hal::QueueSend(void * queue, const uint8_t msg[])
{
// This context ownership should be part of each child class
#if SystemUsesFreeRTOS == 1
    if( xPortInIsrContext() )                               /**< Does this work in another platform rather than ESP32? */
    {    
        BaseType_t xHigherPriorityTaskWoken;
        xQueueSendFromISR((QueueHandle_t)queue,  (void*) msg, &xHigherPriorityTaskWoken);
        if(xHigherPriorityTaskWoken)
            portYIELD_FROM_ISR();
    } 
    else
    {
        xQueueSend((QueueHandle_t)queue, (void*) msg, 0);
    }
#endif


#if SystemUsesZephyrRTOS == 1
    while (k_msgq_put((QueueHandle_t*)queue, msg, K_NO_WAIT) != 0) {   /**< Send data to consumers */
        k_msgq_purge((QueueHandle_t*)queue);			                    /**< Message queue is full: purge old data & try again */
    }            
    LOG_HEXDUMP_DBG(msg, 5, "RTOS::Hal::QueueSend");   
#endif
}
