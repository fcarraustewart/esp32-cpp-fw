// Subscriber.h
#pragma once
#include <string>
#include <functional>
#include "MsgBrokerT.hpp"
#include "Message.hpp"

class Subscriber {
public:
    Subscriber(const std::string& topic, MsgBrokerT* broker, MsgBrokerT::Callback callback)
        : topic(topic), callback(callback) {
        broker->registerSubscriber(topic, this);
    }

    void receive(const Message& message) {
        callback(message);
    }

private:
    std::string topic;
    MsgBrokerT::Callback callback;
};
