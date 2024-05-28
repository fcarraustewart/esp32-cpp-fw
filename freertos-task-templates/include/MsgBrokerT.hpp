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
    int registerSubscriber(const std::string& topic, MsgBrokerT::Callback callback);
    void publish(const Message& message);
    void unsubscribeFrom(const std::string& topic, const size_t id);

private:
    struct SubscriberEntry {
        std::string topic;
        Subscriber* subscriber;
    };

    std::array<SubscriberEntry, 10> subscribers;  // Fixed size array
    size_t subscriberCount = 0;
};
