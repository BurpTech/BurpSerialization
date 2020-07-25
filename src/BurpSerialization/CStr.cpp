#include "CStr.hpp"

namespace BurpSerialization
{

    CStr::CStr(const size_t length, const StatusCodes statusCodes) :
        _length(length),
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code CStr::deserialize(Value & dest, const JsonVariant & src) {
        dest.isNull = true;
        if (src.isNull()) {
            return _statusCodes.notPresent;
        }
        if (src.is<const char *>()) {
            auto value = src.as<const char *>();
            if (strlen(value) > _length) {
                return _statusCodes.tooLong;
            }
            dest.isNull = false;
            dest.cstr = value;
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool CStr::serialize(const JsonVariant & dest, const Value & src) const {
        if (src.isNull) {
            dest.clear();
            return true;
        }
        return dest.set(src.cstr);
    }

}