// MsgBrokerT.h
#pragma once
#include <string>
#include <array>
#include <functional>

class Message;

class MsgBrokerT {
public:
    using Callback = std::function<void(const Message&)>;
    class Subscriber {
    public:
        void receive(const Message& message) {
            callback(message);
        }
        std::string topic = "";
        MsgBrokerT::Callback callback = [](const Message& msg){};
    private:
    };
    MsgBrokerT() {
        for(size_t i = 0; i<10; i++)
            freeID[i] = -1;
    }
    int registerSubscriber(const std::string& topic, MsgBrokerT::Callback callback);
    void publish(const Message& message);
    void unsubscribeFrom(const std::string& topic, const size_t id);

private:
    std::array<Subscriber, 10> subscribers;  // Fixed size array
    size_t subscriberCount = 0;
    std::array<size_t, 10> freeID;
};
