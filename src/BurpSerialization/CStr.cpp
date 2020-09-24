#include "CStr.hpp"

namespace BurpSerialization
{

    CStr::CStr(
        const size_t minLength,
        const size_t maxLength,
        const StatusCodes statusCodes,
        const char *& value
    ) :
        _minLength(minLength),
        _maxLength(maxLength),
        _statusCodes(statusCodes),
        _value(value)
    {}

    BurpStatus::Status::Code CStr::deserialize(const JsonVariant & serialized) const {
        _value = nullptr;
        if (serialized.isNull()) {
            return _statusCodes.notPresent;
        }
        if (serialized.is<const char *>()) {
            auto value = serialized.as<const char *>();
            if (strlen(value) < _minLength) {
                return _statusCodes.tooShort;
            }
            if (strlen(value) > _maxLength) {
                return _statusCodes.tooLong;
            }
            _value = value;
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool CStr::serialize(const JsonVariant & serialized) const {
        if (_value == nullptr) {
            serialized.clear();
            return true;
        }
        return serialized.set(_value);
    }

}