#pragma once

#include "Field.hpp"

namespace BurpSerialization
{
    constexpr size_t MAC_ADDRESS_BYTE_COUNT = 6;

    class MacAddress : public Field
    {

    public:

        struct Value {
            bool isNull = false;
            uint8_t value[MAC_ADDRESS_BYTE_COUNT];
        };

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
            const BurpStatus::Status::Code invalidCharacter;
            const BurpStatus::Status::Code invalidSeparator;
            const BurpStatus::Status::Code outOfRange;
            const BurpStatus::Status::Code missingField;
            const BurpStatus::Status::Code excessCharacters;
        };

        MacAddress(const StatusCodes statusCodes, Value & value);

        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) const override;
        bool serialize(const JsonVariant & serialized) const override;

    private:

        const StatusCodes _statusCodes;
        Value & _value;

    };
    
}