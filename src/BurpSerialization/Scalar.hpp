#pragma once

#include "Field.hpp"

namespace BurpSerialization
{

    template <class Type>
    class Scalar : public Field
    {

    public:

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
        };

        Scalar(const StatusCodes statusCodes) :
            _statusCodes(statusCodes)
        {}

        BurpStatus::Status::Code deserialize(Value & dest, const JsonVariant & src) const override {
            dest.isNull = true;
            if (src.isNull()) {
                return _statusCodes.notPresent;
            }
            if (src.is<Type>()) {
                dest = src.as<Type>();
                return _statusCodes.ok;
            }
            return _statusCodes.wrongType;
        }

        bool serialize(const JsonVariant & dest, const Value & src) const override {
            if (src.isNull) {
                dest.clear();
                return true;
            }
            return dest.set(static_cast<Type>(src));
        }

    private:

        const StatusCodes _statusCodes;

    };
    
}