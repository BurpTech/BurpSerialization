#pragma once

#include <array>
#include "TypedField.hpp"

namespace BurpSerialization
{

    template <class Type, size_t length>
    class Array : public TypedField<std::array<Type, length + 1>>
    {

    public:

        // length + 1 as we are using this as a zero terminated array
        using List = std::array<Type, length + 1>;

        struct StatusCodes {
            const BurpStatus::Status::Code ok;
            const BurpStatus::Status::Code notPresent; // set to ok if not required
            const BurpStatus::Status::Code wrongType;
            const BurpStatus::Status::Code tooLong;
            const BurpStatus::Status::Code zeroEntry;
        };

        Array(TypedField<Type> & field, const StatusCodes statusCodes) :
            _field(field),
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
                const auto jsonArray = serialized.as<JsonArray>();
                const auto size = jsonArray.size();
                if (size > length) {
                    return _statusCodes.tooLong;
                }
                for (size_t index = 0; index < size; index++) {
                    const BurpStatus::Status::Code code = _field.deserialize(jsonArray[index]);
                    if (code != _statusCodes.ok) return code;
                    const auto entry = _field.get();
                    // array is zero terminated so should not contain any zeros
                    if (!entry) return _statusCodes.zeroEntry;
                    _list[index] = entry;
                }
                _present = true;
                return _statusCodes.ok;
            }
            return _statusCodes.wrongType;
        }

        bool serialize(const JsonVariant & serialized) const override {
            if (_present) {
                const JsonArray jsonArray = serialized.to<JsonArray>();
                for (auto entry : _list) {
                    // Zero terminated so return true when no more entries
                    if (!entry) return true;
                    if (!jsonArray.add(entry)) return false;
                }
                // not zero teminated
                return false;
            }
            serialized.clear();
            return true;
        }

        bool isPresent() const {
            return _present;
        }

        void setNotPresent() {
            _present = false;
        }

        List get() const override {
            return _list;
        }

        void set(const List list) override {
            _present = true;
            _list = list;
        }

    private:

        TypedField<Type> & _field;
        List _list;
        bool _present;
        const StatusCodes _statusCodes;

    };
    
}