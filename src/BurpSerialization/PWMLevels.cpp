#include "PWMLevels.hpp"

namespace BurpSerialization
{

    PWMLevels::PWMLevels(const StatusCodes statusCodes) :
        _field({
            statusCodes.ok,
            statusCodes.levelNotPresent,
            statusCodes.levelWrongType
        }),
        _list({}),
        _value({.uint8List=_list.data()}),
        _present(false),
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code PWMLevels::deserialize(const JsonVariant & serialized) {
        _present = false;
        _list.fill({});
        if (serialized.isNull()) {
            return _statusCodes.notPresent;
        }
        if (serialized.is<JsonArray>()) {
            auto jsonArray = serialized.as<JsonArray>();
            auto size = jsonArray.size();
            uint8_t lastLevel = 0;
            if (size > maxLevels) {
                return _statusCodes.tooLong;
            }
            for (size_t index = 0; index < size; index++) {
                auto code = _field.deserialize(jsonArray[index]);
                if (code != _statusCodes.ok) return code;
                auto level = _field.get()->uint8;
                if (level == 0) return _statusCodes.levelZero;
                if (level <= lastLevel) return _statusCodes.levelNotIncreasing;
                lastLevel = level;
                _list[index] = level;
            }
            _present = true;
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool PWMLevels::serialize(const JsonVariant & serialized) const {
        if (_present) {
            auto jsonArray = serialized.to<JsonArray>();
            for (auto level : _list) {
                if (level == 0) return true;
                auto success = jsonArray.add(level);
                if (!success) return false;
            }
            // not zero terminated
            return false;
        }
        serialized.clear();
        return true;
    }

    const Value * PWMLevels::get() const {
        if (_present) {
            return &_value;
        }
        return nullptr;
    }

    void PWMLevels::set(const Value * value) {
        if (value && value->uint8List) {
            _present = true;
            std::copy_n(value->uint8List, _list.size(), _list.begin());
        } else {
            _present = false;
        }
    }

}