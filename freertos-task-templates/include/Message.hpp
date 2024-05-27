// Message.h
#pragma once
#include <string>
#include <cstring>
#include <array>
#include <any>
#include <algorithm>

class Message {
public:
    std::string mTopic;
    std::string mPublisher;
    std::array<std::pair<std::string, std::any>, 10> mEventData;  // Fixed size array
    size_t mDataCount;

    Message(const std::string& mTopic, const std::string& mPublisher)
        : mTopic(mTopic), mPublisher(mPublisher), mDataCount(0) {}

    template<typename T>
    int addEventData(const std::string& key, T value) {
        if (mDataCount < mEventData.size()) {
            mEventData[mDataCount++] = {key, value};
            return 0;
        }
        else {
            return -1;
        }
    }

    template<typename T>
    T getEventData(const std::string& key) const {
        auto it = std::find_if(mEventData.begin(), mEventData.begin() + mDataCount,
                               [&key](const auto& pair) { return pair.first == key; });
        if (it != mEventData.begin() + mDataCount) {
            return std::any_cast<T>(it->second);
        }
        throw std::out_of_range("Key not found");
    }
    void print(char * serviceName) const;
    size_t serialize(uint8_t* buffer, size_t bufferSize) const;
    bool deserialize(const uint8_t* buffer, size_t bufferSize);
};
