#pragma once

#include "Field.hpp"
#include "functional"

namespace BurpSerialization
{

    template <class Type>
    class Scalar : public Field
    {

    public:

        struct Value {
            bool isNull = false;
            Type value = 0;
        };

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
        };

        Scalar(const StatusCodes statusCodes, Value & value) :
            _statusCodes(statusCodes),
            _value(value)
        {}

        BurpStatus::Status::Code deserialize(const JsonVariant & src) const override {
            _value.isNull  = true;
            if (src.isNull()) {
                return _statusCodes.notPresent;
            }
            if (src.is<Type>()) {
                _value.isNull  = false;
                _value.value  = src.as<Type>();
                return _statusCodes.ok;
            }
            return _statusCodes.wrongType;
        }

        bool serialize(const JsonVariant & dest) const override {
            if (_value.isNull) {
                dest.clear();
                return true;
            }
            return dest.set(_value.value);
        }

    private:

        const StatusCodes _statusCodes;
        Value & _value;

    };
    
}