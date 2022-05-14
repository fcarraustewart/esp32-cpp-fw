#ifndef ACTIVE_OBJECT__H_H
#define ACTIVE_OBJECT__H_H


#include "FreeRTOS.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

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
            D::Initialize();
            printf("%s::Initializes.\r\n", mName.c_str());
        };
        static void End()
        {
            D::End();
            printf("%s::Ends.\r\n", mName.c_str());
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
            printf("On %s::Send() %d.\r\n", mName.c_str(), msg[0]);
            xQueueSend(mInputQueue, (void*)msg, 0);
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
