#ifndef ACTIVE_OBJECT__H_H
#define ACTIVE_OBJECT__H_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <string>
#include "Logger.hpp"
#include <thread>
#include <queue>
namespace RTOS
{
    template <class D>
    class ActiveObject
    {
    public:
        static void Create()
        {
            xTaskCreate(&Run, mName.c_str(), 4096, nullptr, 2, &mHandle);
        };
        static void Run(void * arg)
        {
            Logger::Log("Service::%s::Initializes.", mName.c_str());
            D::Initialize();

            while(1)
                Loop();

            Logger::Log("Service::%s::End().", mName.c_str());
            D::End();
        };
        static void Loop()
        {
            if(pdTRUE == xQueueReceive(mInputQueue, (void*)mReceivedMsg, portMAX_DELAY))
                D::Handle(mReceivedMsg);
        };
        static void Send(const uint8_t msg[])
        {
            // This context ownership is part of each child class
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
        ActiveObject() {};

    /**
     *                  Member Variables:
     */
    public:
        static const    std::string     mName;                      /**< The variables used to create the queue */
        static const    uint8_t         mInputQueueItemSize;        /**< The variables used to create the queue */
    protected:
        static          uint8_t         mCountLoops;                /**< The variables used to create the queue */
        static const    uint8_t         mInputQueueItemLength;      /**< The variables used to create the queue */

        
        static const    size_t          mInputQueueSizeBytes;       /**< The variables used to create the queue */
        static          uint8_t         mInputQueueAllocation[];    /**< The variables used to create the queue */

        /** 
         * The variable used to hold the queue's data structure. 
         * */
        static          QueueHandle_t   mInputQueue;

        /** 
         * The handle used to hold the task for this ActiveObject. 
         * */
        static          TaskHandle_t    mHandle;
        static          uint8_t         mReceivedMsg[];
    };

}
#endif
