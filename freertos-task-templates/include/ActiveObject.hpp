#ifndef ACTIVE_OBJECT__H_H
#define ACTIVE_OBJECT__H_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <string>

namespace RTOS
{
    template <class D>
    class ActiveObject
    {
    public:
        static void Run(void * arg)
        {
            Initialize();
            while(1)
                Loop();
            End();
        };
        static void Initialize()
        {
            printf("Service::%s::Initializes.\r\n", mName.c_str());
            D::Initialize();
        };
        static void End()
        {
            printf("Service::%s::End().\r\n", mName.c_str());
            D::End();
        };
        static void Loop()
        {
            uint8_t arg[mInputQueueItemSize];
            if(pdTRUE == xQueueReceive(mInputQueue, (void*)arg, portMAX_DELAY))
                D::Loop(arg);
        };
        static void Send(const uint8_t msg[])
        {
            if( xPortInIsrContext() ) /**< Does this work in another platform rather than ESP32? */
            {    
                BaseType_t xHigherPriorityTaskWoken;
                xQueueSendFromISR(mInputQueue,  (void*) msg, &xHigherPriorityTaskWoken);
                if(xHigherPriorityTaskWoken)
                    portYIELD_FROM_ISR();
            } 
            else
            {
                xQueueSend(mInputQueue,         (void*) msg, portMAX_DELAY);
            }
        };

    private:
    protected:
        ActiveObject(){};

    /**
     *                  Member Variables:
     */
    public:
    protected:
        static const    std::string mName;                      /**< The variables used to create the queue */
        static          uint8_t     mCountLoops;                /**< The variables used to create the queue */
        static const    uint8_t     mInputQueueItemLength;      /**< The variables used to create the queue */

        
        static const    uint8_t     mInputQueueItemSize;        /**< The variables used to create the queue */
        static const    size_t      mInputQueueSizeBytes;       /**< The variables used to create the queue */
        static          uint8_t     mInputQueueAllocation[];    /**< The variables used to create the queue */

        /** 
         * The variable used to hold the queue's data structure. 
         * */
        static          QueueHandle_t   mInputQueue;
    };

}
#endif
