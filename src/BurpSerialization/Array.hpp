#pragma once

#include "ValueList.hpp"
#include "Field.hpp"

namespace BurpSerialization
{

    template <size_t length>
    class Array : public Field
    {

    public:

        using List = ValueList<length>;

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
            const BurpStatus::Status::Code tooLong;
        };

        Array(Field & field, const StatusCodes statusCodes) :
            _field(field),
            _list({}),
            _value({.list=_list.data()}),
            _present(false),
            _statusCodes(statusCodes)
        {}

        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) override {
            _present = false;
            _list.fill({});
            if (serialized.isNull()) {
                return _statusCodes.notPresent;
            }
            if (serialized.is<JsonArray>()) {
                auto jsonArray = serialized.as<JsonArray>();
                auto size = jsonArray.size();
                if (size > length) {
                    return _statusCodes.tooLong;
                }
                for (size_t index = 0; index < size; index++) {
                    auto code = _field.deserialize(jsonArray[index]);
                    if (code != _statusCodes.ok) return code;
                    _list[index] = _field.get();
                }
                _present = true;
                return _statusCodes.ok;
            }
            return _statusCodes.wrongType;
        }

        bool serialize(const JsonVariant & serialized) const override {
            if (_present) {
                auto jsonArray = serialized.to<JsonArray>();
                for (size_t index = 0; index < length; index++) {
                    _field.set(_list[index]);
                    if (!_field.serialize(jsonArray[index])) return false;
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
                std::copy_n(value->list, length, _list.begin());
            } else {
                _present = false;
            }
        }

    private:

        Field & _field;
        List _list;
        const Value _value;
        bool _present;
        const StatusCodes _statusCodes;

    };
    
}