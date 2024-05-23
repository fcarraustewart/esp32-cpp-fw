// MsgBrokerT.h
#pragma once
#include <string>
#include <array>
#include <functional>

class Message;
class Subscriber;

class MsgBrokerT {
public:
    using Callback = std::function<void(const Message&)>;

    void registerSubscriber(const std::string& topic, Subscriber* subscriber);
    void publish(const Message& message);

private:
    struct SubscriberEntry {
        std::string topic;
        Subscriber* subscriber;
    };

    std::array<SubscriberEntry, 10> subscribers;  // Fixed size array
    size_t subscriberCount = 0;
};
