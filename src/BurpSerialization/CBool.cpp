#include "CBool.hpp"

namespace BurpSerialization
{

    CBool::CBool(const StatusCodes statusCodes) :
        _value(false),
        _exists(false),
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code CBool::deserialize(const JsonVariant & serialized) {
        _exists = false;
        if (serialized.isNull()) {
            return _statusCodes.notPresent;
        }
        if (serialized.is<bool>()) {
            _exists = true;
            _value = serialized.as<bool>();
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool CBool::serialize(const JsonVariant & serialized) const {
        if (_exists) {
            return serialized.set(_value);
        }
        serialized.clear();
        return true;
    }

    bool CBool::exists() const {
        return _exists;
    }

    void CBool::exists(const bool exists) {
        _exists = exists;
    }

    bool CBool::get() const {
        return _value;
    }

    void CBool::set(const bool value) {
        _value = value;
    }

}