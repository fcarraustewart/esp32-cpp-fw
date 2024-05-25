#include "System.hpp"
#include <exception>
#include <array>

#define TIMER_DIVIDER 64
static uint8_t messageForLEDsService[2]={ADD_TO_BLINK_COLOR_OPCODE, 0x00};

static Service::HardwareTimers::TimerEvt  timer_requested      = Service::HardwareTimers::TimerEvt();
static Message                            topic_timer_100us    = Message("", "");
static uint64_t                           counter_requested    = 0;

void Service::HardwareTimers::Initialize()
{
    mTimer = timerBegin(0, TIMER_DIVIDER, true);
    timerAttachInterrupt(mTimer, &Service::HardwareTimers::OnTimer, true);
    timerAlarmWrite(mTimer, 1000000, true);
    timerAlarmEnable(mTimer);
// #define EVENTS_INTERESTED RTOS::MsgBroker::Event::BLE_Connected , ...
// System::mMsgBroker::Subscribe<EVENTS_INTERESTED>();
}

void Service::HardwareTimers::Handle(const uint8_t arg[]){
    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
        case REQUEST_TIMER_MSG_PUBLISHED_CMD:
        {
            /*
             Start publishing procedure:
                * Save variables from the request
                    * We received a request and topic to be published: - Maybe from a Service or MsgBroker
                        * Parse request: 
                        * TimerEvent = arg[1..7]
                        * Message = arg[8..14]
                    * Create a new TimerEvt in the array
                        * Set TimerCountUp value
                        * Set TimerMode either [OneShot, Periodic]
                        * Set TimerUnit [ms,us,s]
                        * Move state of timer to Running
                    * Install an interrupt accordingly to the new TimerEvt and topic requested
                    * Next interrupts
                        * Decrease TimerCountUp value
                        * Check TimerMode to see how many times we need to publish repetitions
                        * Finish and change state if needed
                    * Check for out of bounds
            */

            Logger::Log("[Service::%s]::%s().\t Got Message.", mName.c_str(), __func__);
            for(size_t i=1; i<RTOS::MsgBroker::cMaxPayloadLength-1; i+=4)
                Logger::Log("[Service::%s]::%s().\t payload[%d,%d,%d,%d] = %c%c%c%c = 0x%x%x%x%x.", mName.c_str(), __func__, i,i+1,i+2,i+3, arg[i], arg[i+1], arg[i+2], arg[i+3], arg[i], arg[i+1], arg[i+2], arg[i+3]);

            if(topic_timer_100us.deserialize(&arg[1], RTOS::MsgBroker::cMaxPayloadLength-1))
            {
                Logger::Log("[Service::%s]::%s().\t deserialized %s.", mName.c_str(), __func__, topic_timer_100us.mTopic.c_str());
                Logger::Log("[Service::%s]::%s().\t deserialized %s.", mName.c_str(), __func__, topic_timer_100us.mPublisher.c_str());
                Logger::Log("[Service::%s]::%s().\t deserialized %d.", mName.c_str(), __func__, topic_timer_100us.mDataCount);
                Logger::Log("[Service::%s]::%s().\t deserialized %s.", mName.c_str(), __func__, topic_timer_100us.getEventData<std::string>("Cmd").c_str());
                Logger::Log("[Service::%s]::%s().\t deserialized %s.", mName.c_str(), __func__, topic_timer_100us.getEventData<std::string>("mCountUp").c_str());    
                Logger::Log("[Service::%s]::%s().\t deserialized %s.", mName.c_str(), __func__, topic_timer_100us.getEventData<std::string>("mState").c_str()); 
                Logger::Log("[Service::%s]::%s().\t deserialized %s.", mName.c_str(), __func__, topic_timer_100us.getEventData<std::string>("mMode").c_str()); 
                Logger::Log("[Service::%s]::%s().\t deserialized %s.", mName.c_str(), __func__, topic_timer_100us.getEventData<std::string>("mUnit").c_str());    
                 
            }

            timer_requested.mState = TimerState::Running;
            counter_requested = timer_requested.mCountUp;
            mEventScheduler.at(0).first = counter_requested;
            mEventScheduler.at(0).second = timer_requested;

        }  break;
        case 5:
        {
            // Timer100us
            // Iterate through the list
            for(auto event : mEventScheduler) {
                counter_requested = mEventScheduler.at(0).first; // TODO: use event instead
                timer_requested = mEventScheduler.at(0).second; // TODO: use event instead

                if(timer_requested.mState == TimerState::Running && timer_requested.mUnit == TimerUnit::us) {
                    if(counter_requested > 0) {
                        counter_requested--;
                    }   
                    if(counter_requested == 0) {
                        // Event finished, go ahead an publish the notification:
                        Message msg = Message("Timer100us", "HardwareTimers");
                        int err = msg.addEventData("Cmd",    std::to_string(REQUEST_TIMER_MSG_PUBLISHED_CMD));
                        err = msg.addEventData("mCountUp",   std::to_string(timer_requested.mCountUp));
                        err = msg.addEventData("mMode",      std::to_string((uint8_t)timer_requested.mMode));
                        err = msg.addEventData("mState",     std::to_string((uint8_t)timer_requested.mState));
                        err = msg.addEventData("mUnit",      std::to_string((uint8_t)timer_requested.mUnit));
                        System::mMsgBroker.mIPC.publishEvent( "Timer100us", msg );

                        if(timer_requested.mMode == TimerMode::Periodic) {
                            counter_requested = timer_requested.mCountUp;
                        } 
                        else {
                            timer_requested.mState = TimerState::Done;
                            Logger::Log("[Service::%s]::%s().\t Timer Done and published %s.", mName.c_str(), __func__, msg.mTopic.c_str());
                            break;
                        }
                    }
                    mEventScheduler.at(0).first = counter_requested;// TODO: use event instead
                    mEventScheduler.at(0).second = timer_requested;// TODO: use event instead
                }

            }               
            
            try
            {
                // The parameter of at(i) should be i = 1 because that's the position where we put LoRa in the variant.
                auto x = std::get<Service::LEDs>(System::mSystemServicesRegistered.at(3));
                //messageForLEDsService[0]=arg[0] % 2 == 0 ? ADD_TO_BLINK_COLOR_OPCODE : RESET_BLINK_COLOR_OPCODE;
                messageForLEDsService[0]=FIRE_BLINK_COLOR_OPCODE;
                messageForLEDsService[1]=arg[0];
                Logger::Log("[Service::%s]::%s():\t%x. Pass to LEDs.", mName.c_str(), __func__, arg[0]);
                x.Send(messageForLEDsService);
            }
            catch (std::bad_variant_access const& ex)
            {
                Logger::Log("Bad Variant Access -> %s.", ex.what());
            }            
            try
            {
                // The parameter of at(i) should be i = 1 because that's the position where we put LoRa in the variant.
                auto x = std::get<Service::BLE>(System::mSystemServicesRegistered.at(0));
                Logger::Log("[Service::%s]::%s():\t%x. Pass to BLE.", mName.c_str(), __func__, arg[0]);
                x.Send(arg);
            }
            catch (std::bad_variant_access const& ex)
            {
                Logger::Log("Bad Variant Access -> %s.", ex.what());
            }
            
            break;
        }
        default:
        {
            Logger::Log("[Service::%s]::%s():\t%x.\tNYI.", mName.c_str(), __func__, arg[0]);
            break;
        }
    };
}



/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using               _HardwareTimers = RTOS::ActiveObject<Service::HardwareTimers>;

    template <>
    const std::string   _HardwareTimers::mName = std::string("HardwareTimers");
    template <>
    uint8_t             _HardwareTimers::mCountLoops = 0;
    template <>
    const uint8_t       _HardwareTimers::mInputQueueItemLength = 16;
    template <>
    const uint8_t       _HardwareTimers::mInputQueueItemSize = RTOS::MsgBroker::cMaxPayloadLength;
    template <>
    const size_t        _HardwareTimers::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize;
    template <>
    uint8_t             _HardwareTimers::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    QueueHandle_t       _HardwareTimers::mInputQueue = xQueueCreate(
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize
                                    );
    template <>
    TaskHandle_t        _HardwareTimers::mHandle = 0;
    template <>
    uint8_t             _HardwareTimers::mReceivedMsg[
                                        RTOS::ActiveObject<Service::HardwareTimers>::mInputQueueItemSize
                                    ] = { 0 };
}

/** 
 * Build specific Service static members
 *  Otherwise it complains that the RTOS 
 * activeObject does not have these members, which is true.
*/
namespace Service
{
    volatile uint16_t   HardwareTimers::mInterruptCounter = 0;
    hw_timer_t*         HardwareTimers::mTimer = nullptr;
    std::array<std::pair<uint64_t, HardwareTimers::TimerEvt>, 10> HardwareTimers::mEventScheduler = {
        std::make_pair(1, HardwareTimers::TimerEvt()),
        std::make_pair(1, HardwareTimers::TimerEvt()),
        std::make_pair(1, HardwareTimers::TimerEvt()),
        std::make_pair(1, HardwareTimers::TimerEvt()),
        std::make_pair(1, HardwareTimers::TimerEvt()),
        std::make_pair(1, HardwareTimers::TimerEvt()),
        std::make_pair(1, HardwareTimers::TimerEvt()),
        std::make_pair(1, HardwareTimers::TimerEvt()),
        std::make_pair(1, HardwareTimers::TimerEvt()),
        std::make_pair(1, HardwareTimers::TimerEvt())
    };  // Fixed size array
}
