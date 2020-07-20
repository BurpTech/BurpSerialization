#pragma once

#include "Field.hpp"

namespace BurpSerialization
{

    class UInt8 : public Field
    {

    public:

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
        };

        UInt8(const StatusCodes statusCodes);

        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) override;
        bool serialize(const JsonVariant & serialized) const override;
        const Value * get() const override;
        void set(const Value * value) override;

    private:

        Value _value;
        bool _present;
        const StatusCodes _statusCodes;

    };
    
}