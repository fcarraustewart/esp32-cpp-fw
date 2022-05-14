#ifndef ACTIVE_OBJECT__H_H
#define ACTIVE_OBJECT__H_H
#include "FreeRTOS.h"
namespace RTOS
{

    template <class D>
    class ActiveObject
    {
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
            D::Loop();
            printf("%s::Loops %d.\r\n", mName.c_str(), mCountLoops);
            mCountLoops++;
        };

    private:
    protected:
        ActiveObject(){};
        static const std::string mName;
        static const uint8_t mInputQueueItemLength;
        static const uint8_t mInputQueueItemSize;
        static uint8_t mInputQueueAllocation[];
        static uint8_t mCountLoops;
    };

}
#endif
