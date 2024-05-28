// MsgBrokerT.cpp
#include "MsgBrokerT.hpp"
#include "Message.hpp"
#include "Subscriber.hpp"

int MsgBrokerT::registerSubscriber(const std::string& topic, MsgBrokerT::Callback callback) {
    if (subscriberCount < subscribers.size()) {
        Subscriber* subscriber = new Subscriber(topic, callback);
        subscribers[subscriberCount++] = {topic, subscriber};
        return subscriberCount-1;
    } else
    {
        return -1;
    }
}

void MsgBrokerT::publish(const Message& message) {
    const auto& topic = message.mTopic;
    for (size_t i = 0; i < subscriberCount; ++i) {
        if (subscribers[i].topic == topic) {
            subscribers[i].subscriber->receive(message);
        }
    }
}

void MsgBrokerT::unsubscribeFrom(const std::string& topic, const size_t id) {
    if (id < subscribers.size()) {
        delete subscribers[id].subscriber;
        subscribers[id].topic = "";
    }
}