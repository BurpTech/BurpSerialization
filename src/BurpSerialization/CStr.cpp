#include "CStr.hpp"

namespace BurpSerialization
{

    CStr::CStr(const size_t length, const bool required, const StatusCodes statusCodes) :
        _length(length),
        _required(required),
        _statusCodes(statusCodes)
    {}

    const BurpStatus::Status::Code CStr::deserialize(const JsonVariant & serialized) {
        _value = nullptr;
        if (serialized.isNull()) {
            if (_required) {
                return _statusCodes.notPresent;
            }
            return _statusCodes.ok;
        }
        if (serialized.is<const char *>()) {
            const char * value = serialized.as<const char *>();
            if (strlen(value) > _length) {
                return _statusCodes.tooLong;
            }
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    void CStr::serialize(const JsonVariant & serialized) const {
    }

    const char * CStr::get() const {
        return _value;
    }

    void CStr::set(const char * value) {
        _value = value;
    }

}