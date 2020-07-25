#pragma once

#include "Field.hpp"

namespace BurpSerialization
{

    class IPv4 : public Field
    {

    public:

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
            const BurpStatus::Status::Code invalidCharacter;
            const BurpStatus::Status::Code outOfRange;
            const BurpStatus::Status::Code missingField;
            const BurpStatus::Status::Code excessCharacters;
        };

        IPv4(const StatusCodes statusCodes);

        BurpStatus::Status::Code deserialize(Value & dest, const JsonVariant & src) const override;
        bool serialize(const JsonVariant & dest, const Value & src) const override;

    private:

        const StatusCodes _statusCodes;

    };
    
}