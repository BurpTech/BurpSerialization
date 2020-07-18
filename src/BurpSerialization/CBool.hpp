#pragma once

#include "TypedField.hpp"

namespace BurpSerialization
{

    class CBool : public TypedField<bool>
    {

    public:

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
        };

        CBool(const StatusCodes statusCodes);

        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) override;
        bool serialize(const JsonVariant & serialized) const override;
        bool isPresent() const;
        void setNotPresent();
        bool get() const override;
        void set(const bool value) override;

    private:

        bool _value;
        bool _present;
        const StatusCodes _statusCodes;

    };
    
}