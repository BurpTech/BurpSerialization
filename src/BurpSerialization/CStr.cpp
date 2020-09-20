#include "CStr.hpp"

namespace BurpSerialization
{

    CStr::CStr(const size_t length, const StatusCodes statusCodes, const char *& value) :
        _length(length),
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
            if (strlen(value) > _length) {
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