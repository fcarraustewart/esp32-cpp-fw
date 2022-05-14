#ifndef ACTIVE_OBJECT__H_H
#define ACTIVE_OBJECT__H_H
#include "FreeRTOS.h"
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
            printf("%s::Loops %d.\r\n", mName.c_str(), mCountLoops);
            mCountLoops++;
            D::Loop();
        };

    private:
    protected:
        ActiveObject(){};

    /**
     *                  Member Variables:
     */
    public:
        static const uint8_t        mInputQueueItemLength;
        static const uint8_t        mInputQueueItemSize;
        static const size_t         mInputQueueSizeBytes;
        static uint8_t              mInputQueueAllocation[];
    protected:
        static const std::string    mName;
        static uint8_t              mCountLoops;
    };

}
#endif
