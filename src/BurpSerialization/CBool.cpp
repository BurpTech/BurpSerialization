#include "CBool.hpp"

namespace BurpSerialization
{

    CBool::CBool(const StatusCodes statusCodes) :
        _value({.cbool=false}),
        _present(false),
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code CBool::deserialize(const JsonVariant & serialized) {
        _present = false;
        _value.cbool = false;
        if (serialized.isNull()) {
            return _statusCodes.notPresent;
        }
        if (serialized.is<bool>()) {
            _present = true;
            _value.cbool = serialized.as<bool>();
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool CBool::serialize(const JsonVariant & serialized) const {
        if (_present) {
            return serialized.set(_value.cbool);
        }
        serialized.clear();
        return true;
    }

    const Value * CBool::get() const {
        if (_present) {
            return &_value;
        }
        return nullptr;
    }

    void CBool::set(const Value * value) {
        if (value) {
            _present = true;
            _value = *value;
        } else {
            _present = false;
        }
    }

}