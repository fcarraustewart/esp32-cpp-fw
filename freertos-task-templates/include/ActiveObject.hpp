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
    /**
     * Methods:
     */
    public:
        static void Run()
        static void Run(void * arg)
        {
            Initialize();
            while(1)
                Loop();
            End();
        };
        static void Initialize()
        {
            printf("%s::Initializes.\r\n", mName.c_str());
            D::Initialize();
        };
        static void End()
        {
            printf("%s::Ends.\r\n", mName.c_str());
            D::End();
        };
        static void Loop()
        {
            uint8_t arg[mInputQueueItemSize];
            
            printf("Service::%s::Loop() %d.\r\n", mName.c_str(), mCountLoops);
            mCountLoops++;
    
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
                printf("On Service::%s::Send(). Posting message: 0x", mName.c_str());
                for (size_t i = 0; i < mInputQueueItemSize; i++)
                    printf("%x", msg[i]);
                printf("\r\n");
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
