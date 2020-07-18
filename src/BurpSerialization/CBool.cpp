#include "CBool.hpp"

namespace BurpSerialization
{

    CBool::CBool(const StatusCodes statusCodes) :
        _value(false),
        _present(false),
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code CBool::deserialize(const JsonVariant & serialized) {
        _present = false;
        _value = false;
        if (serialized.isNull()) {
            return _statusCodes.notPresent;
        }
        if (serialized.is<bool>()) {
            _present = true;
            _value = serialized.as<bool>();
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool CBool::serialize(const JsonVariant & serialized) const {
        if (_present) {
            return serialized.set(_value);
        }
        serialized.clear();
        return true;
    }

    bool CBool::isPresent() const {
        return _present;
    }

    void CBool::setNotPresent() {
        _present = false;
    }

    bool CBool::get() const {
        return _value;
    }

    void CBool::set(const bool value) {
        _present = true;
        _value = value;
    }

}