#include "UInt32.hpp"

namespace BurpSerialization
{

    UInt32::UInt32(const StatusCodes statusCodes) :
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code UInt32::deserialize(Value & dest, const JsonVariant & src) const {
        dest.isNull = true;
        if (src.isNull()) {
            return _statusCodes.notPresent;
        }
        if (src.is<uint32_t>()) {
            dest.isNull = false;
            dest.uint32 = src.as<uint32_t>();
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool UInt32::serialize(const JsonVariant & dest, const Value & src) const {
        if (src.isNull) {
            dest.clear();
            return true;
        }
        return dest.set(src.uint32);
    }

}