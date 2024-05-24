// MsgBrokerT.cpp
#include "MsgBrokerT.hpp"
#include "Message.hpp"
#include "Subscriber.hpp"

void MsgBrokerT::registerSubscriber(const std::string& topic, Subscriber* subscriber) {
    if (subscriberCount < subscribers.size()) {
        subscribers[subscriberCount++] = {topic, subscriber};
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
