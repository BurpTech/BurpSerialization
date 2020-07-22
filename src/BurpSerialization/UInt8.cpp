#include "UInt8.hpp"

namespace BurpSerialization
{

    UInt8::UInt8(const StatusCodes statusCodes) :
        _value({.uint8=0}),
        _present(false),
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code UInt8::deserialize(const JsonVariant & serialized) {
        _present = false;
        _value.uint8 = 0;
        if (serialized.isNull()) {
            return _statusCodes.notPresent;
        }
        if (serialized.is<uint8_t>()) {
            _present = true;
            _value.uint8 = serialized.as<uint8_t>();
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool UInt8::serialize(const JsonVariant & serialized) const {
        if (_present) {
            return serialized.set(_value.uint8);
        }
        serialized.clear();
        return true;
    }

    const Value * UInt8::get() const {
        if (_present) {
            return &_value;
        }
        return nullptr;
    }

    void UInt8::set(const Value * value) {
        if (value) {
            _present = true;
            _value = *value;
        } else {
            _present = false;
        }
    }

}