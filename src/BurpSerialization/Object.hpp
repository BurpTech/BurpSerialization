#pragma once

#include <array>
#include "Value.hpp"
#include "Field.hpp"

namespace BurpSerialization
{

    template <size_t entryCount>
    class Object : public Field
    {

    public:

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
        };

        struct Entry {
            const char * name;
            Field * field;
        };
        using Entries = std::array<Entry, entryCount>;
        using List = std::array<Value, entryCount>;

        Object(const Entries entries, const StatusCodes statusCodes) :
            _entries(entries),
            _statusCodes(statusCodes)
        {}

        BurpStatus::Status::Code deserialize(Value & dest, const JsonVariant & src) const override {
            dest.isNull = true;
            if (src.isNull()) {
                return _statusCodes.notPresent;
            }
            if (src.is<JsonObject>()) {
                for (size_t index = 0; index < entryCount; index++) {
                    auto entry = _entries[index];
                    auto code = entry.field->deserialize(dest.valueList[index], src[entry.name]);
                    if (code != _statusCodes.ok) return code;
                }
                dest.isNull = false;
                return _statusCodes.ok;
            }
            return _statusCodes.wrongType;
        }

        bool serialize(const JsonVariant & dest, const Value & src) const override {
            if (src.isNull) {
                dest.clear();
                return true;
            }
            for (size_t index = 0; index < entryCount; index++) {
                auto entry = _entries[index];
                if (!entry.field->serialize(dest[entry.name].template to<JsonVariant>(), src.valueList[index])) return false;
            }
            return true;
        }

    private:

        const Entries _entries;
        const StatusCodes _statusCodes;

    };
    
}