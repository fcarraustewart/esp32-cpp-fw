// Message.h
#pragma once
#include <string>
#include <array>
#include <any>
#include <algorithm>

class Message {
public:
    std::string topic;
    std::string publisher;
    std::array<std::pair<std::string, std::any>, 10> eventData;  // Fixed size array
    size_t dataCount;

    Message(const std::string& topic, const std::string& publisher)
        : topic(topic), publisher(publisher), dataCount(0) {}

    template<typename T>
    void addEventData(const std::string& key, T value) {
        if (dataCount < eventData.size()) {
            eventData[dataCount++] = {key, value};
        }
    }

    template<typename T>
    T getEventData(const std::string& key) const {
        auto it = std::find_if(eventData.begin(), eventData.begin() + dataCount,
                               [&key](const auto& pair) { return pair.first == key; });
        if (it != eventData.begin() + dataCount) {
            return std::any_cast<T>(it->second);
        }
        throw std::out_of_range("Key not found");
    }
};
