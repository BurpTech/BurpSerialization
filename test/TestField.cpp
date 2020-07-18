#include "TestField.hpp"

TestField::TestField(const StatusCodes statusCodes) :
    _statusCodes(statusCodes),
    _value(nullptr)
{}

BurpStatus::Status::Code TestField::deserialize(const JsonVariant & serialized) {
    _value = nullptr;
    if (serialized.isNull()) {
        return _statusCodes.notPresent;
    }
    if (serialized.is<const char *>()) {
        _value = serialized.as<const char *>();
        return _statusCodes.ok;
    }
    return _statusCodes.wrongType;
}

bool TestField::serialize(const JsonVariant & serialized) const {
    return serialized.set(_value);
}

const char * TestField::get() const {
    return _value;
}

void TestField::set(const char * value) {
    _value = value;
}