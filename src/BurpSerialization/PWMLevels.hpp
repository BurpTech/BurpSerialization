#pragma once

#include <array>
#include "UInt8.hpp"
#include "Field.hpp"

namespace BurpSerialization
{

    class PWMLevels : public Field
    {

    public:

        static constexpr size_t maxLevels = 255;
        using List = std::array<uint8_t, maxLevels + 1>;

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
            const BurpStatus::Status::Code tooLong;
            const BurpStatus::Status::Code levelZero;
            const BurpStatus::Status::Code levelNotIncreasing;
            const BurpStatus::Status::Code levelNotPresent;
            const BurpStatus::Status::Code levelWrongType;
        };

        PWMLevels(const StatusCodes statusCodes);

        BurpStatus::Status::Code deserialize(Value & dest, const JsonVariant & src) override;
        bool serialize(const JsonVariant & dest, const Value & src) const override;

    private:

        UInt8 _field;
        const StatusCodes _statusCodes;

    };
    
}