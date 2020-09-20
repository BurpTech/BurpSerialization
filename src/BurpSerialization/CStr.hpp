#pragma once

#include "Field.hpp"

namespace BurpSerialization
{

    class CStr : public Field
    {

    public:

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
            const BurpStatus::Status::Code tooLong;
        };

        CStr(const size_t length, const StatusCodes statusCodes, const char *& value);

        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) const override;
        bool serialize(const JsonVariant & serialized) const override;

    private:

        const size_t _length;
        const StatusCodes _statusCodes;
        const char *& _value;

    };
    
}