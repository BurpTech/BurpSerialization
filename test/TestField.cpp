#include "TestField.hpp"

TestField::TestField(const StatusCodes statusCodes) :
    _statusCodes(statusCodes),
    _value({.cstr=nullptr})
{}

BurpStatus::Status::Code TestField::deserialize(const JsonVariant & serialized) {
    _value.cstr = nullptr;
    if (serialized.isNull()) {
        return _statusCodes.notPresent;
    }
    if (serialized.is<const char *>()) {
        _value.cstr = serialized.as<const char *>();
        return _statusCodes.ok;
    }
    return _statusCodes.wrongType;
}

bool TestField::serialize(const JsonVariant & serialized) const {
    return serialized.set(_value.cstr);
}

const BurpSerialization::Value * TestField::get() const {
    if (_value.cstr) {
        return &_value;
    }
    return nullptr;
}

void TestField::set(const BurpSerialization::Value * value) {
    if (value && value->cstr) {
        _value = *value;
    } else {
        _value.cstr = nullptr;
    }
}