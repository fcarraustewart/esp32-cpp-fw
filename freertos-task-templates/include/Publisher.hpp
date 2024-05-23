// Publisher.h
#pragma once
#include <string>
#include "MsgBrokerT.hpp"
#include "Message.hpp"

class Publisher {
public:
    Publisher(const std::string& id, MsgBrokerT* broker)
        : id(id), broker(broker) {}

    void publish(const Message& message) {
        broker->publish(message);
    }

    std::string getId() const {
        return id;
    }

private:
    std::string id;
    MsgBrokerT* broker;
};
