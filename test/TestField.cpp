#include "TestField.hpp"

TestField::TestField(const StatusCodes statusCodes, const char *& value) :
    _statusCodes(statusCodes),
    _value(value)
{}

BurpStatus::Status::Code TestField::deserialize(const JsonVariant & src) const {
    _value = nullptr;
    if (src.isNull()) {
        return _statusCodes.notPresent;
    }
    if (src.is<const char *>()) {
        _value = src.as<const char *>();
        return _statusCodes.ok;
    }
    return _statusCodes.wrongType;
}

bool TestField::serialize(const JsonVariant & dest) const {
    if (_value == nullptr) {
        dest.clear();
        return true;
    }
    return dest.set(_value);
}