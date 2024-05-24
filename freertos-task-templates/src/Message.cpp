#include "Message.hpp"
#include "Logger.hpp"

size_t Message::serialize(uint8_t* buffer, size_t bufferSize) const {
    size_t offset = 0;

    // Serialize mTopic
    size_t topicSize = mTopic.size();
    if (offset + sizeof(topicSize) + topicSize > bufferSize) return 0;
    std::memcpy(buffer + offset, &topicSize, sizeof(topicSize));
    offset += sizeof(topicSize);
    std::memcpy(buffer + offset, mTopic.c_str(), topicSize);
    offset += topicSize;

    // Serialize mPublisher
    size_t publisherSize = mPublisher.size();
    if (offset + sizeof(publisherSize) + publisherSize > bufferSize) return 0;
    std::memcpy(buffer + offset, &publisherSize, sizeof(publisherSize));
    offset += sizeof(publisherSize);
    std::memcpy(buffer + offset, mPublisher.c_str(), publisherSize);
    offset += publisherSize;

    // Serialize mEventData count
    if (offset + sizeof(mDataCount) > bufferSize) return 0;
    std::memcpy(buffer + offset, &mDataCount, sizeof(mDataCount));
    offset += sizeof(mDataCount);

    // Serialize mEventData
    for (size_t i = 0; i < mDataCount; ++i) {
        const auto& key = mEventData[i].first;
        const auto& value = mEventData[i].second;

        // Serialize key
        size_t keySize = key.size();
        if (offset + sizeof(keySize) + keySize > bufferSize) return 0;
        std::memcpy(buffer + offset, &keySize, sizeof(keySize));
        offset += sizeof(keySize);
        std::memcpy(buffer + offset, key.c_str(), keySize);
        offset += keySize;

        // Serialize value as string (assuming value is always string for simplicity)
        // You should handle different types in a more complex implementation
        try {
            const std::string& strValue = std::any_cast<const std::string&>(value);
            size_t valueSize = strValue.size();
            if (offset + sizeof(valueSize) + valueSize > bufferSize) return 0;
            std::memcpy(buffer + offset, &valueSize, sizeof(valueSize));
            offset += sizeof(valueSize);
            std::memcpy(buffer + offset, strValue.c_str(), valueSize);
            offset += valueSize;
        } catch (const std::bad_any_cast&) {
            Logger::Log("[Message.cpp].\t ERROR: Caught Exception: Bad Any Cast at serialize().");   
            return 0;
        }
    }

    return offset;
}

bool Message::deserialize(const uint8_t* buffer, size_t bufferSize) {
    size_t offset = 0;

    // Deserialize mTopic
    size_t topicSize;
    if (offset + sizeof(topicSize) > bufferSize) return false;
    std::memcpy(&topicSize, buffer + offset, sizeof(topicSize));
    offset += sizeof(topicSize);
    if (offset + topicSize > bufferSize) return false;
    mTopic = std::string(reinterpret_cast<const char*>(buffer + offset), topicSize);
    offset += topicSize;

    // Deserialize mPublisher
    size_t publisherSize;
    if (offset + sizeof(publisherSize) > bufferSize) return false;
    std::memcpy(&publisherSize, buffer + offset, sizeof(publisherSize));
    offset += sizeof(publisherSize);
    if (offset + publisherSize > bufferSize) return false;
    mPublisher = std::string(reinterpret_cast<const char*>(buffer + offset), publisherSize);
    offset += publisherSize;
 
    // Deserialize mEventData count, this is the Message received DataCount
    size_t receivedMessageDataCount = 0;
    if (offset + sizeof(mDataCount) > bufferSize) return false;
    std::memcpy(&receivedMessageDataCount, buffer + offset, sizeof(mDataCount));
    offset += sizeof(mDataCount);

    // Deserialize mEventData
    for (size_t i = 0; i < receivedMessageDataCount; ++i) {
        // Deserialize key
        size_t keySize;
        if (offset + sizeof(keySize) > bufferSize) return false;
        std::memcpy(&keySize, buffer + offset, sizeof(keySize));
        offset += sizeof(keySize);
        if (offset + keySize > bufferSize) return false;
        std::string key(reinterpret_cast<const char*>(buffer + offset), keySize);
        offset += keySize;

        // Deserialize value as string (assuming value is always string for simplicity)
        // You should handle different types in a more complex implementation
        size_t valueSize;
        if (offset + sizeof(valueSize) > bufferSize) return false;
        std::memcpy(&valueSize, buffer + offset, sizeof(valueSize));
        offset += sizeof(valueSize);
        if (offset + valueSize > bufferSize) return false;
        std::string value(reinterpret_cast<const char*>(buffer + offset), valueSize);
        offset += valueSize;

        addEventData(key, value); // This increments mDataCount
    }

    return true;
}


/* Example Usage */

void sendMessage(const Message& message) {
    uint8_t buffer[256]; // Adjust size as needed
    size_t size = message.serialize(buffer, sizeof(buffer));
    if (size > 0) {
        if (1) {
            // Handle send error
        }
    } else {
        // Handle serialization error
    }
}
void receiveMessage() {
    uint8_t buffer[256]; // Adjust size as needed
    if (1) {
        Message message("","");
        if (message.deserialize(buffer, sizeof(buffer))) {
            // Process the message
        } else {
            // Handle deserialization error
        }
    } else {
        // Handle receive error
    }
}