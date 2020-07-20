#pragma once

#include "ValueList.hpp"
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
        using List = ValueList<entryCount>;

        Object(const Entries entries, const StatusCodes statusCodes) :
            _entries(entries),
            _list({}),
            _value({.list=_list.data()}),
            _present(false),
            _statusCodes(statusCodes)
        {}

        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) override {
            _present = false;
            if (serialized.isNull()) {
                return _statusCodes.notPresent;
            }
            if (serialized.is<JsonObject>()) {
                for (size_t index = 0; index < entryCount; index++) {
                    auto entry = _entries[index];
                    auto code = entry.field->deserialize(serialized[entry.name]);
                    if (code != _statusCodes.ok) return code;
                    _list[index] = entry.field->get();
                }
                _present = true;
                return _statusCodes.ok;
            }
            return _statusCodes.wrongType;
        }

        bool serialize(const JsonVariant & serialized) const override {
            if (_present) {
                for (size_t index = 0; index < entryCount; index++) {
                    auto entry = _entries[index];
                    entry.field->set(_list[index]);
                    if (!entry.field->serialize(serialized[entry.name].template to<JsonVariant>())) return false;
                }
                return true;
            }
            serialized.clear();
            return true;
        }

        const Value * get() const override {
            if (_present) {
                return &_value;
            }
            return nullptr;
        }

        void set(const Value * value) override {
            if (value && value->list) {
                _present = true;
                std::copy_n(value->list, entryCount, _list.begin());
            } else {
                _present = false;
            }
        }

    private:

        const Entries _entries;
        List _list;
        const Value _value;
        bool _present;
        const StatusCodes _statusCodes;

    };
    
}