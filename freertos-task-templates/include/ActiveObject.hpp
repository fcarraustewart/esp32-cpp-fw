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
        {
            Initialize();
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
            
            xQueueReceive(mInputQueue, (void*)arg, portMAX_DELAY);
            printf("%s::Loops %d.\r\n", mName.c_str(), mCountLoops);
            mCountLoops++;
            
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
        static const std::string    mName;
        static uint8_t              mCountLoops;

        /* The variables used to create the queue */
        static const uint8_t        mInputQueueItemLength;
        static const uint8_t        mInputQueueItemSize;
        static const size_t         mInputQueueSizeBytes;
        static uint8_t              mInputQueueAllocation[];

        /* The variable used to hold the queue's data structure. */
        static QueueHandle_t        mInputQueue;
    };

}
#endif
