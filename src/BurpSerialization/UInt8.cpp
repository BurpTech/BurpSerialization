#include "UInt8.hpp"

namespace BurpSerialization
{

    UInt8::UInt8(const StatusCodes statusCodes) :
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code UInt8::deserialize(Value & dest, const JsonVariant & src) {
        dest.isNull = true;
        if (src.isNull()) {
            return _statusCodes.notPresent;
        }
        if (src.is<uint8_t>()) {
            dest.isNull = false;
            dest.uint8 = src.as<uint8_t>();
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool UInt8::serialize(const JsonVariant & dest, const Value & src) const {
        if (src.isNull) {
            dest.clear();
            return true;
        }
        return dest.set(src.uint8);
    }

}