#pragma once
#include <string>
#include <array>
#include "MsgBrokerT.hpp"
#include "Publisher.hpp"

class IPC {
public:
    IPC(const std::string& id, MsgBrokerT* broker)
        : id(id), broker(broker), publisher(id, broker) {}

    void publishEvent(const std::string& topic, const Message& message) {
        Message msg(topic, id);
        msg = message;
        publisher.publish(msg);
    }

    int subscribeTo(const std::string& topic, MsgBrokerT::Callback callback) {
        return broker-> registerSubscriber(topic, callback);
    }

    void unsubscribeFrom(const std::string& topic, const size_t id) {
        broker->unsubscribeFrom(topic, id);
    }

    ~IPC() {
    }

private:
    std::string id;
    MsgBrokerT* broker;
    Publisher publisher;
};
