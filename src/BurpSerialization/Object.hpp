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
            Field * field;
        };
        using Entries = std::array<Entry, entryCount>;

        Object(const Entries entries, const StatusCodes statusCodes) :
            _entries(entries),
            _present(false),
            _statusCodes(statusCodes)
        {}

        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) override {
            _present = false;
            if (serialized.isNull()) {
                return _statusCodes.notPresent;
            }
            if (serialized.is<JsonObject>()) {
                for (auto entry : _entries) {
                    const BurpStatus::Status::Code code = entry.field->deserialize(serialized[entry.name]);
                    if (code != _statusCodes.ok) return code;
                }
                _present = true;
                return _statusCodes.ok;
            }
            return _statusCodes.wrongType;
        }

        bool serialize(const JsonVariant & serialized) const override {
            if (_present) {
                for (auto entry : _entries) {
                    const bool success = entry.field->serialize(serialized[entry.name].template to<JsonVariant>());
                    if (!success) return false;
                }
                return true;
            }
            serialized.clear();
            return true;
        }

        bool isPresent() const {
            return _present;
        }

        void setPresent(const bool present) {
            _present = present;
        }

    private:

        const Entries _entries;
        bool _present;
        const StatusCodes _statusCodes;

    };
    
}