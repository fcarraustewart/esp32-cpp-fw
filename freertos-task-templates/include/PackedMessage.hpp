#include <cstdint>
#include <cstring>
#include <array>
#include <string>
#include "MsgBroker.hpp"
#define FIX_EVENT_DATA_KEY_VALUE_MAX_SIZE 10

struct PackedMessage {
    char mTopic[RTOS::MsgBroker::cMaxPayloadLength];
    char mPublisher[RTOS::MsgBroker::cMaxPayloadLength];
    uint8_t mDataCount;
    std::array<std::pair<std::array<char, RTOS::MsgBroker::cMaxPayloadLength>, std::array<char, RTOS::MsgBroker::cMaxPayloadLength>>, FIX_EVENT_DATA_KEY_VALUE_MAX_SIZE> mEventData;

    PackedMessage() : mDataCount(0) {
        std::memset(mTopic, 0, sizeof(mTopic));
        std::memset(mPublisher, 0, sizeof(mPublisher));
    }

    void setTopic(const std::string& newTopic) {
        std::strncpy(mTopic, newTopic.c_str(), sizeof(mTopic) - 1);
    }

    void setPublisher(const std::string& newPublisher) {
        std::strncpy(mPublisher, newPublisher.c_str(), sizeof(mPublisher) - 1);
    }

    void addEventData(const std::string& key, const std::string& value) {
        if (mDataCount < mEventData.size()) {
            std::strncpy(mEventData[mDataCount].first.data(), key.c_str(), mEventData[mDataCount].first.size() - 1);
            std::strncpy(mEventData[mDataCount].second.data(), value.c_str(), mEventData[mDataCount].second.size() - 1);
            mDataCount++;
        }
    }

    size_t serialize(uint8_t* buffer, size_t bufferSize) const {
        if (bufferSize < sizeof(PackedMessage)) {
            return 0;
        }
        std::memcpy(buffer, this, sizeof(PackedMessage));
        return sizeof(PackedMessage);
    }

    bool deserialize(const uint8_t* buffer, size_t bufferSize) {
        if (bufferSize < sizeof(PackedMessage)) {
            return false;
        }
        std::memcpy(this, buffer, sizeof(PackedMessage));
        return true;
    }
} __attribute__((packed));


/*
    Example usage:
*/

void sendMessage(const Message& message) {
    //if ((xQueue, &message, pdMS_TO_TICKS(100)) != pdPASS) {
        // Handle send error
    //}
}


void receiveMessage() {
    // Message message;
    //if (xQueueReceive(xQueue, &message, pdMS_TO_TICKS(100)) == pdPASS) {
        // Process the message
    //} else {
        // Handle receive error
    //}
}