#pragma once

#include <array>
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
            const Field * field;
        };
        using Entries = std::array<Entry, entryCount>;

        Object(const Entries entries, const StatusCodes statusCodes, bool & isNull) :
            _entries(entries),
            _statusCodes(statusCodes),
            _isNull(isNull)
        {}

        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) const override {
            _isNull = true;
            if (serialized.isNull()) {
                return _statusCodes.notPresent;
            }
            if (serialized.is<JsonObject>()) {
                auto ret = _statusCodes.ok;
                for (auto entry : _entries) {
                    auto code = entry.field->deserialize(serialized[entry.name]);
                    if (code != _statusCodes.ok) ret = code;
                }
                _isNull = false;
                return ret;
            }
            return _statusCodes.wrongType;
        }

        bool serialize(const JsonVariant & serialized) const override {
            if (_isNull) {
                serialized.clear();
                return true;
            }
            for (auto entry : _entries) {
                if (!entry.field->serialize(serialized[entry.name].template to<JsonVariant>())) return false;
            }
            return true;
        }

    private:

        const Entries _entries;
        const StatusCodes _statusCodes;
        bool & _isNull;

    };
    
}