#ifndef ACTIVE_OBJECT__H_H
#define ACTIVE_OBJECT__H_H

#include <hal/RTOS.hpp>
//#include <Logger.hpp>

namespace RTOS
{
	// Work in progress: adding concepts to the ActiveObject
	template <typename T>
	concept StaticBinding_Impl = requires (T t) 
	{ 
		{ t.Initialize () } -> std::same_as < void >;
		{ t.Loop () } -> std::same_as < void >;
		{ t.Handle () } -> std::same_as < void >;
		{ t.End () } -> std::same_as < void >;
	};

    template <class D>
    class ActiveObject
    {
    public:
		constexpr ActiveObject() {Create();};
		
		~ActiveObject() = default;

        static constexpr bool Create()
        {
			auto res = mHandle.set_name(mName);
			if(res.has_value() == false)
				return false;
			return RTOS::Hal::TaskCreate(&Run, mName, &mHandle);
        };
        static constexpr void Run(void) noexcept
        {
            D::Initialize();

            while(1)
                Loop();

            D::End();
        };
        static constexpr void Loop()
        {
            if(true == RTOS::Hal::QueueReceive(&mInputQueue, (void*)mReceivedMsg))
                D::Handle(mReceivedMsg);
        };
        static inline void Send(const uint8_t msg[])
        {
            RTOS::Hal::QueueSend(&mInputQueue, msg);
        };
    private:
    protected:

    /**
     *                  Member Variables:
     */
    public:
        static const    char         	mName[];                    /**< The variables used to create the queue */
        static const    uint8_t         mInputQueueItemSize;        /**< The variables used to create the queue */
    protected:
        static          uint8_t         mCountLoops;                /**< The variables used to create the queue */
        static const    uint8_t         mInputQueueItemLength;      /**< The variables used to create the queue */

        
        static const    size_t          mInputQueueSizeBytes;       /**< The variables used to create the queue */
        static          char            mInputQueueAllocation[];    /**< The variables used to create the queue */

        /** 
         * The variable used to hold the queue's data structure. 
         * */
        static          RTOS::QueueHandle_t   mInputQueue;

        /** 
         * The handle used to hold the task for this ActiveObject. 
         * */
        static          RTOS::TaskHandle_t  mHandle;
        static          uint8_t             mReceivedMsg[];
        
        static          zpp::thread_stack   mTaskStack;
        static          zpp::thread_data    mTaskControlBlock;

    };

}
#endif
