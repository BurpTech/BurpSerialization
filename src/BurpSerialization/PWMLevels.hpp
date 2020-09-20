#pragma once

#include <array>
#include "Scalar.hpp"
#include "Field.hpp"

namespace BurpSerialization
{

    class PWMLevels : public Field
    {

    public:

        static constexpr size_t maxLevels = 255;
        using List = std::array<uint8_t, maxLevels + 1>;

        struct Value {
            bool isNull;
            List list;
        };

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

        PWMLevels(const StatusCodes statusCodes, Value & value);

        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) const override;
        bool serialize(const JsonVariant & serialized) const override;

    private:

        Scalar<uint8_t> _uint8Field;
        Scalar<uint8_t>::Value _temp;
        const StatusCodes _statusCodes;
        Value & _value;

    };
    
}