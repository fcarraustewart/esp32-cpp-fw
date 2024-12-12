#define SLEEPTIME  500
#define STACKSIZE 512
#define NUM_THREADS 3
#define TCOUNT 10
#define COUNT_LIMIT 12

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(Hal);

#include "hal/RTOS.hpp"
#include <Utils/zpp.hpp>
#include <Utils/TaggedCounter.hpp>


struct ThreadCounter {};
namespace {
    ZPP_KERNEL_STACK_ARRAY_DEFINE(tstack, NUM_THREADS, STACKSIZE);
    zpp::thread_data tcb[NUM_THREADS];
    zpp::thread t[NUM_THREADS];
} // anonimouse namespace
// ZPP_KERNEL_STACK_DEFINE(tstack1, STACKSIZE);
// ZPP_KERNEL_STACK_DEFINE(tstack2, STACKSIZE);

void RTOS::Hal::TaskCreate(func_t thread, const uint8_t name[], TaskHandle_t* handle)
{
#if SystemUsesFreeRTOS == 1
    xTaskCreate(&thread, name, 4096, 
            nullptr, 2, 
            &handle);
#endif

#if SystemUsesZephyrRTOS == 1
    //ARG_UNUSED(handle);
    LOG_INF("Initializing: %s", name);
    
    //
    // Create thread attributes used for thread creation
    //
    const zpp::thread_attr my_thread_attr(
            zpp::thread_prio::preempt(0),
            zpp::thread_inherit_perms::no,
            zpp::thread_suspend::no
        );
    constexpr int d0 = Meta::TaggedCounter<ThreadCounter>::Value();
    constexpr int d1 = Meta::TaggedCounter<ThreadCounter>::Value();



    if(0 == strcmp((char*)name,"LoRa")) 
        t[0] = zpp::thread(tcb[d0], tstack(d0), my_thread_attr, thread);

    if(0 == strcmp((char*)name,"HardwareTimers"))
        t[1] = zpp::thread(tcb[d1], tstack(d1), my_thread_attr, thread);

    mThreadCount = mThreadCount + 1;
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
    if(0 == k_msgq_get((QueueHandle_t*)queue, receivedMsg, K_FOREVER)) {            
        LOG_HEXDUMP_DBG(receivedMsg, 5, "RTOS::Hal::QueueReceive");          
        //LOG_INF("RTOS::Hal::QueueReceive from address=%d", this);
        return true;
    }
#endif
    return false;
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

uint8_t RTOS::Hal::mThreadCount = 0;