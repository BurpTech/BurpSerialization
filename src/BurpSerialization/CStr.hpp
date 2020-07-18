#pragma once

#include "Field.hpp"

namespace BurpSerialization
{

    class CStr : public Field
    {

    public:

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // not used if not required
            const BurpStatus::Status::Code wrongType;
            const BurpStatus::Status::Code tooLong;
        };

        CStr(const size_t length, const bool required, const StatusCodes statusCodes);

        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) override;
        bool serialize(const JsonVariant & serialized) const override;
        const char * get() const;
        void set(const char * value);

    private:

        const char * _value;
        const size_t _length;
        const bool _required;
        const StatusCodes _statusCodes;

    };
    
}