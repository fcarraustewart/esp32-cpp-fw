#include <Services/HardwareTimers.hpp>

#include <System.hpp>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(HardwareTimers, LOG_LEVEL_INF);

namespace {
	#define CMD_HW_TIMER_1 0x11
	static struct k_timer timer;
	static volatile uint16_t count = 0;
	static uint8_t msgforHWTimers[] = {CMD_HW_TIMER_1, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0};
	static uint8_t msgforlora[] = {0x04,0x03,0x02,0x01,0x06};
	static uint8_t msgforLEDsLEDShow[] = {CMD_LEDs_SHOW,0x03,0x02,0x01,0x06};
	static uint8_t msgforLEDsBuzzer[] = {CMD_WORKQUEUE_SONG,0x00,0x02,0x01,0x06};
	static uint8_t msgforLEDsLogPanic[] = {0x66,0,0,0,0};
	volatile k_timer_expiry_t timer_expiry_fn = [](struct k_timer *timer_id) {
		Service::HardwareTimers::Send(msgforHWTimers);
	};
}



void Service::HardwareTimers::Initialize() {
    // #define EVENTS_INTERESTED RTOS::MsgBroker::Event::BLE_Connected , ...
    // System::mMsgBroker::Subscribe<EVENTS_INTERESTED>();        	
    LOG_INF("%s: HardwareTimers Module Initialized correctly.", __FUNCTION__);

	zpp::this_thread::sleep_for(std::chrono::milliseconds(1000));

	k_timer_init(&timer, timer_expiry_fn, NULL);

	k_timer_start(&timer, K_MSEC(60), K_TIMEOUT_ABS_MS(10));

	zpp::this_thread::set_priority(zpp::thread_prio::preempt(2));
}

void Service::HardwareTimers::Handle(const uint8_t arg[]) {
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
		case CMD_HW_TIMER_1:
		{
			count++;
			// uint16_t m_count = *(uint16_t*)&arg[1];
			if(count%4 == 0)
			{
				LOG_DBG("msgforHWTimers %d times.", count);
				Service::IMU::Send(msgforHWTimers);
			}
			if(count%89 == 0)
			{
				LOG_DBG("msgforHWTimers %d times.", count);
				Service::Sensor::Send(msgforHWTimers);
			}
			if(count%77 == 0)
			{
				LOG_DBG("msgforLEDsLEDShow %d times.", count);
				Service::LEDs::Send(msgforLEDsLEDShow);
			}
			if(count%4096 == 0) {
				msgforLEDsBuzzer[1] = 4;
				Service::LEDs::Send(msgforLEDsBuzzer);
				Service::LoRa::Send(msgforlora);
			}
			if(count%8000 == 0) {
				Service::LEDs::Send(msgforLEDsLogPanic);
			}
			if(count == 12000) {
				msgforLEDsBuzzer[1] = 0;
				Service::LEDs::Send(msgforLEDsBuzzer);
				count = 0;
			}
			
		}; break;
        default:
        {
            LOG_DBG("[Service::%s]::%s():\t%x.\tNYI.", mName, __func__, arg[0]);   
            LOG_HEXDUMP_DBG(arg, 5, "\t\t\t HardwareTimers msg Buffer values.");
            break;
        }
    };
}

/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using                       _HardwareTimers = RTOS::ActiveObject<Service::HardwareTimers>;

    template <>
    const char               	_HardwareTimers::mName[] =  "HardwareTimers";
    template <>
    uint8_t                     _HardwareTimers::mCountLoops = 0;
    template <>
    const uint8_t               _HardwareTimers::mInputQueueItemLength = 16;
    template <>
    const uint8_t               _HardwareTimers::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t                _HardwareTimers::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize;
    template <>
    char                        _HardwareTimers::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    RTOS::QueueHandle_t         _HardwareTimers::mInputQueue = RTOS::Hal::QueueCreate(
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize,
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueAllocation
                                    );
    template <>
    uint8_t                     _HardwareTimers::mReceivedMsg[
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemLength
                                    ] = { 0 };

    namespace {
    ZPP_KERNEL_STACK_DEFINE(hwtimersstack, 768);
    template <>
    zpp::thread_data            _HardwareTimers::mTaskControlBlock = zpp::thread_data();
    template <>
    zpp::thread                 _HardwareTimers::mHandle = zpp::thread(
                                        mTaskControlBlock, 
                                        Service::hwtimersstack(), 
                                        RTOS::cThreadAttributes, 
                                        Service::_HardwareTimers::Run
                                    );
    } //https://www.reddit.com/r/cpp/comments/4ukhh5/what_is_the_purpose_of_anonymous_namespaces/#:~:text=The%20purpose%20of%20an%20anonymous,will%20not%20have%20internal%20linkage.



                                    
}
