#include "CBool.hpp"

namespace BurpSerialization
{

    CBool::CBool(const StatusCodes statusCodes) :
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code CBool::deserialize(Value & dest, const JsonVariant & src) {
        dest.isNull = true;
        if (src.isNull()) {
            return _statusCodes.notPresent;
        }
        if (src.is<bool>()) {
            dest.isNull = false;
            dest.cbool = src.as<bool>();
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool CBool::serialize(const JsonVariant & dest, const Value & src) const {
        if (src.isNull) {
            dest.clear();
            return true;
        }
        return dest.set(src.cbool);
    }

}