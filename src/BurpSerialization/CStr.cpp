#include "CStr.hpp"

namespace BurpSerialization
{

    CStr::CStr(const size_t length, const StatusCodes statusCodes) :
        _value({.cstr=nullptr}),
        _length(length),
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code CStr::deserialize(const JsonVariant & serialized) {
        _value.cstr = nullptr;
        if (serialized.isNull()) {
            return _statusCodes.notPresent;
        }
        if (serialized.is<const char *>()) {
            auto value = serialized.as<const char *>();
            if (strlen(value) > _length) {
                return _statusCodes.tooLong;
            }
            _value.cstr = value;
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool CStr::serialize(const JsonVariant & serialized) const {
        return serialized.set(_value.cstr);
    }

    const Value * CStr::get() const {
        if (_value.cstr) {
            return &_value;
        }
        return nullptr;
    }

    void CStr::set(const Value * value) {
        if (value && value->cstr) {
            _value = *value;
        } else {
            _value.cstr = nullptr;
        }
    }

}