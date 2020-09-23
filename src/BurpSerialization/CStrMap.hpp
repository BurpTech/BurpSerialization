#pragma once

#include <array>
#include "Field.hpp"

namespace BurpSerialization
{

    template <class Type, size_t count>
    class CStrMap : public Field
    {

    public:

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
            const BurpStatus::Status::Code invalidChoice;
        };

        struct Value {
            bool isNull = false;
            Type value;
        };

        struct Choice {
            const char * key;
            Type value;
        };

        using Choices = std::array<Choice, count>;

        CStrMap(Choices choices, const StatusCodes statusCodes, Value & value) :
            _choices(choices),
            _statusCodes(statusCodes),
            _value(value)
        {}

        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) const override {
            _value.isNull = true;
            if (serialized.isNull()) {
                return _statusCodes.notPresent;
            }
            if (serialized.is<const char *>()) {
                auto value = serialized.as<const char *>();
                for (auto choice : _choices) {
                    if (strcmp(choice.key, value) == 0) {
                        _value.isNull = false;
                        _value.value = choice.value;
                        return _statusCodes.ok;
                    }
                }
                return _statusCodes.invalidChoice;
            }
            return _statusCodes.wrongType;
        }

        bool serialize(const JsonVariant & serialized) const override {
            serialized.clear();
            if (_value.isNull) {
                return true;
            }
            for (auto choice : _choices) {
                if (choice.value == _value.value) {
                    return serialized.set(choice.key);
                }
            }
            return false;
        }

    private:

        const Choices _choices;
        const StatusCodes _statusCodes;
        Value & _value;

    };
    
}