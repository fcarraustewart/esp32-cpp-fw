// MsgBrokerT.cpp
#include "MsgBrokerT.hpp"
#include "Message.hpp"
#include "Logger.hpp"

int MsgBrokerT::registerSubscriber(const std::string& topic, MsgBrokerT::Callback callback) {
    if (subscriberCount < subscribers.size()) {

        for(size_t i = 0; i<10; i++) {
            if(freeID[i] != -1) {
                size_t subscriberReusedPosition = freeID[i];
                subscribers[subscriberReusedPosition].topic = topic;
                subscribers[subscriberReusedPosition].callback = callback;
                freeID[i] = -1;
                return subscriberReusedPosition;
            }
        }       
        
        subscribers[subscriberCount].topic = topic;
        subscribers[subscriberCount].callback = callback;
        subscriberCount++;
        return subscriberCount-1;
    
    } else {
        return -1;
    }
}

void MsgBrokerT::publish(const Message& message) {
    const auto& topic = message.mTopic;
    for (size_t i = 0; i < subscriberCount; ++i) {
        if (subscribers[i].topic == topic) {
            subscribers[i].receive(message);
        }
    }
}

void MsgBrokerT::unsubscribeFrom(const std::string& topic, const size_t id) {
    static size_t count = 0;
    if (id < subscribers.size()) {
        subscribers[id].topic = "";
        subscribers[id].callback = [](const Message& msg){};
        freeID[count++] = id;
        count%=10;
    }
}