#pragma once

#include "TypedField.hpp"

namespace BurpSerialization
{

    class CStr : public TypedField<const char *>
    {

    public:

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
            const BurpStatus::Status::Code tooLong;
        };

        CStr(const size_t length, const StatusCodes statusCodes);

        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) override;
        bool serialize(const JsonVariant & serialized) const override;
        const char * get() const override;
        void set(const char * value) override;

    private:

        const char * _value;
        const size_t _length;
        const StatusCodes _statusCodes;

    };
    
}