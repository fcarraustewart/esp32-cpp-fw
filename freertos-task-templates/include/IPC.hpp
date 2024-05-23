// IPC.h
#pragma once
#include <string>
#include <array>
#include "MsgBrokerT.hpp"
#include "Publisher.hpp"
#include "Subscriber.hpp"

class IPC {
public:
    IPC(const std::string& id, MsgBrokerT* broker)
        : id(id), broker(broker), publisher(id, broker), subscriberCount(0) {}

    void publishEvent(const std::string& topic, const Message& message) {
        Message msg(topic, id);
        msg = message;
        publisher.publish(msg);
    }

    void subscribeTo(const std::string& topic, MsgBrokerT::Callback callback) {
        if (subscriberCount < subscribers.size()) {
            subscribers[subscriberCount] = new Subscriber(topic, broker, callback);
            ++subscriberCount;
        }
    }

    ~IPC() {
        for (size_t i = 0; i < subscriberCount; ++i) {
            delete subscribers[i];
        }
    }

private:
    std::string id;
    MsgBrokerT* broker;
    Publisher publisher;
    std::array<Subscriber*, 10> subscribers;  // Fixed size array of pointers
    size_t subscriberCount;
};
