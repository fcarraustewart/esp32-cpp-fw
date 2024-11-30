#define SLEEPTIME  500
#define STACKSIZE 4096

#include "hal/RTOS.hpp"

K_THREAD_STACK_DEFINE(stack, 1024);

void RTOS::Hal::TaskCreate(TaskFunction_t thread, const uint8_t name[], TaskHandle_t* handle)
{
#if SystemUsesFreeRTOS == 1
    xTaskCreate(&thread, name, 4096, 
            nullptr, 2, 
            &handle);
#endif

#if SystemUsesZephyrRTOS == 1
    k_thread_create(handle, stack, STACKSIZE,
            (k_thread_entry_t) thread,
            NULL, NULL, NULL, K_PRIO_COOP(7), 0, K_NO_WAIT);
#endif
}

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
bool RTOS::Hal::QueueReceive(void * queue, void * receivedMsg)
{
#if SystemUsesFreeRTOS == 1
    if(pdTRUE == xQueueReceive((QueueHandle_t)queue, (void*)receivedMsg, portMAX_DELAY))
        return true;
    return false;
#endif

#if SystemUsesZephyrRTOS == 1
    k_msgq_get((QueueHandle_t*)queue, &receivedMsg, K_FOREVER);            
    return true;
#endif
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
    while (k_msgq_put((QueueHandle_t*)queue, &msg, K_NO_WAIT) != 0) {   /**< Send data to consumers */
        k_msgq_purge((QueueHandle_t*)queue);			                    /**< Message queue is full: purge old data & try again */
    }
#endif
}

