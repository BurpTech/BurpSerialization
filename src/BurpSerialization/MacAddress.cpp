#include "MacAddress.hpp"

namespace BurpSerialization
{
    char hexDigit(uint8_t value) {
        if (value < 10) {
            return '0' + value;
        }
        return 'A' + (value - 10);
    }

    int8_t hexValue(char digit) {
        if (digit >= '0' && digit <= '9') {
            return digit - '0';
        }
        if (digit >= 'a' && digit <= 'f') {
            return 10 + digit - 'a';
        }
        if (digit >= 'A' && digit <= 'F') {
            return 10 + digit - 'A';
        }
        return -1;
    }

    char * uint8ToHex(uint8_t src, char * dest) {
        uint8_t ones = src % 0x10;
        uint8_t sixteens = src - ones;
        *dest = hexDigit(sixteens / 0x10);
        dest++;
        *dest = hexDigit(ones);
        dest++;
        return dest;
    }

    BurpStatus::Status::Code hexToUInt8(const char *& pos, uint8_t & dest, MacAddress::StatusCodes statusCodes) {
        uint16_t value = 0;
        auto start = pos;
        int8_t digit = hexValue(*pos);
        while (digit > -1) {
            value *= 0x10;
            value += digit;
            pos++;
            digit = hexValue(*pos);
            if (value > 255) return statusCodes.outOfRange;
        }
        if (pos == start) return statusCodes.missingField;
        dest = value;
        return statusCodes.ok;
    }

    BurpStatus::Status::Code strToMacAddress(const char * src, uint8_t dest[MAC_ADDRESS_BYTE_COUNT], MacAddress::StatusCodes statusCodes) {
        auto pos = src;
        char separator = ':';
        for (uint8_t field = 0; field < MAC_ADDRESS_BYTE_COUNT; field++) {
            auto code = hexToUInt8(pos, dest[field], statusCodes);
            if (code != statusCodes.ok) return code;
            if (field == 0) {
                if (*pos == ':') separator = ':';
                else if (*pos == '-') separator = '-';
                else return statusCodes.invalidCharacter;
            }
            else if (field < MAC_ADDRESS_BYTE_COUNT - 1) {
                if (*pos != separator) return statusCodes.invalidSeparator;
            } else {
                if (*pos != 0) return statusCodes.excessCharacters;
            }
            pos++;
        }
        return statusCodes.ok;
    }

    MacAddress::MacAddress(const StatusCodes statusCodes, Value & value) :
        _statusCodes(statusCodes),
        _value(value)
    {}

    BurpStatus::Status::Code MacAddress::deserialize(const JsonVariant & serialized) const {
        _value.isNull = true;
        if (serialized.isNull()) {
            return _statusCodes.notPresent;
        }
        if (serialized.is<const char *>()) {
            auto code = strToMacAddress(serialized.as<const char *>(), _value.value, _statusCodes);
            if (code != _statusCodes.ok) return code;
            _value.isNull = false;
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool MacAddress::serialize(const JsonVariant & serialized) const {
        if (_value.isNull) {
            serialized.clear();
            return true;
        }
        char szMacAddress[MAC_ADDRESS_BYTE_COUNT * 3] = {};
        char * pos = szMacAddress;
        for (uint8_t field = 0; field < MAC_ADDRESS_BYTE_COUNT; field++) {
            if (field > 0) {
                *pos = ':';
                pos++;
            }
            pos = uint8ToHex(_value.value[field], pos);
        }
        return serialized.set(szMacAddress);
    }

}