#include "TestField.hpp"

TestField::TestField(const StatusCodes statusCodes) :
    _statusCodes(statusCodes)
{}

BurpStatus::Status::Code TestField::deserialize(BurpSerialization::Value & dest, const JsonVariant & src) {
    dest.isNull = true;
    if (src.isNull()) {
        return _statusCodes.notPresent;
    }
    if (src.is<const char *>()) {
        dest.isNull = false;
        dest.cstr = src.as<const char *>();
        return _statusCodes.ok;
    }
    return _statusCodes.wrongType;
}

bool TestField::serialize(const JsonVariant & dest, const BurpSerialization::Value & src) const {
    if (src.isNull) {
        dest.clear();
        return true;
    }
    return dest.set(src.cstr);
}