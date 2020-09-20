#include "PWMLevels.hpp"

namespace BurpSerialization
{

    PWMLevels::PWMLevels(const StatusCodes statusCodes, Value & value) :
        _uint8Field({
            statusCodes.ok,
            statusCodes.levelNotPresent,
            statusCodes.levelWrongType
        }, _temp),
        _statusCodes(statusCodes),
        _value(value)
    {}

    BurpStatus::Status::Code PWMLevels::deserialize(const JsonVariant & serialized) const {
        _value.isNull = true;
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
            for (size_t index = 0; index < maxLevels + 1; index++) {
                if (index < size) {
                    auto code = _uint8Field.deserialize(jsonArray[index]);
                    if (code != _statusCodes.ok) return code;
                    auto level = _temp.isNull ? 0 : _temp.value;
                    if (level == 0) return _statusCodes.levelZero;
                    if (level <= lastLevel) return _statusCodes.levelNotIncreasing;
                    lastLevel = level;
                    _value.list[index] = level;
                } else {
                    // initialize to zeros to ensure zero termination
                    _value.list[index] = 0;
                }
            }
            _value.isNull = false;
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool PWMLevels::serialize(const JsonVariant & serialized) const {
        if (_value.isNull) {
            serialized.clear();
            return true;
        }
        auto jsonArray = serialized.to<JsonArray>();
        for (size_t index = 0; index < maxLevels + 1; index++) {
            auto level = _value.list[index];
            if (level == 0) return true;
            auto success = jsonArray.add(level);
            if (!success) return false;
        }
        // not zero terminated
        return false;
    }

}