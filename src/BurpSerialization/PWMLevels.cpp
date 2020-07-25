#include "PWMLevels.hpp"

namespace BurpSerialization
{

    PWMLevels::PWMLevels(const StatusCodes statusCodes) :
        _field({
            statusCodes.ok,
            statusCodes.levelNotPresent,
            statusCodes.levelWrongType
        }),
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code PWMLevels::deserialize(Value & dest, const JsonVariant & src) {
        dest.isNull = true;
        if (src.isNull()) {
            return _statusCodes.notPresent;
        }
        if (src.is<JsonArray>()) {
            auto jsonArray = src.as<JsonArray>();
            auto size = jsonArray.size();
            uint8_t lastLevel = 0;
            if (size > maxLevels) {
                return _statusCodes.tooLong;
            }
            Value value;
            for (size_t index = 0; index < size; index++) {
                auto code = _field.deserialize(value, jsonArray[index]);
                if (code != _statusCodes.ok) return code;
                auto level = value.isNull ? 0 : value.uint8;
                if (level == 0) return _statusCodes.levelZero;
                if (level <= lastLevel) return _statusCodes.levelNotIncreasing;
                lastLevel = level;
                dest.uint8List[index] = level;
            }
            dest.isNull = false;
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool PWMLevels::serialize(const JsonVariant & dest, const Value & src) const {
        if (src.isNull) {
            dest.clear();
            return true;
        }
        auto jsonArray = dest.to<JsonArray>();
        for (size_t index = 0; index < maxLevels + 1; index++) {
            auto level = src.uint8List[index];
            if (level == 0) return true;
            auto success = jsonArray.add(level);
            if (!success) return false;
        }
        // not zero terminated
        return false;
    }

}