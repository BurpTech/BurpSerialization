#include "UInt32.hpp"

namespace BurpSerialization
{

    UInt32::UInt32(const StatusCodes statusCodes) :
        _value({.uint32=0}),
        _present(false),
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code UInt32::deserialize(const JsonVariant & serialized) {
        _present = false;
        _value.uint32 = false;
        if (serialized.isNull()) {
            return _statusCodes.notPresent;
        }
        if (serialized.is<uint32_t>()) {
            _present = true;
            _value.uint32 = serialized.as<uint32_t>();
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool UInt32::serialize(const JsonVariant & serialized) const {
        if (_present) {
            return serialized.set(_value.uint32);
        }
        serialized.clear();
        return true;
    }

    const Value * UInt32::get() const {
        if (_present) {
            return &_value;
        }
        return nullptr;
    }

    void UInt32::set(const Value * value) {
        if (value) {
            _present = true;
            _value = *value;
        } else {
            _present = false;
        }
    }

}