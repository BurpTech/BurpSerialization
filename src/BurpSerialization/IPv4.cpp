#include "IPv4.hpp"

namespace BurpSerialization
{

    char * uint8ToStr(uint8_t src, char * dest) {
        uint8_t ones = src % 10;
        uint8_t tens = (src - ones) % 100;
        uint8_t hundreds = src - tens - ones;
        if (hundreds > 0) {
            *dest = '0' + hundreds / 100;
            dest++;
            *dest = '0' + tens / 10;
            dest++;
            *dest = '0' + ones;
        } else if (tens > 0) {
            *dest = '0' + tens / 10;
            dest++;
            *dest = '0' + ones;
        } else {
            *dest = '0' + ones;
        }
        return dest + 1;
    }

    BurpStatus::Status::Code strToUInt8(const char *& pos, uint32_t * dest, IPv4::StatusCodes statusCodes) {
        uint16_t value = 0;
        auto start = pos;
        while (*pos >= '0' && *pos <= '9') {
            value *= 10;
            value += (*pos) - '0';
            pos++;
            if (value > 255) return statusCodes.outOfRange;
        }
        if (pos == start) return statusCodes.missingField;
        *dest += value;
        return statusCodes.ok;
    }

    BurpStatus::Status::Code strToIPv4(const char * src, uint32_t * dest, IPv4::StatusCodes statusCodes) {
        *dest = 0;
        auto pos = src;
        for (uint8_t field = 0; field < IPV4_BYTE_COUNT; field++) {
            *dest *= 256;
            auto code = strToUInt8(pos, dest, statusCodes);
            if (code != statusCodes.ok) return code;
            if (field < IPV4_BYTE_COUNT - 1) {
                if (*pos != '.') return statusCodes.invalidCharacter;
            } else {
                if (*pos != 0) return statusCodes.excessCharacters;
            }
            pos++;
        }
        return statusCodes.ok;
    }

    IPv4::IPv4(const StatusCodes statusCodes, Value & value) :
        _statusCodes(statusCodes),
        _value(value)
    {}

    BurpStatus::Status::Code IPv4::deserialize(const JsonVariant & serialized) const {
        _value.isNull = true;
        if (serialized.isNull()) {
            return _statusCodes.notPresent;
        }
        if (serialized.is<const char *>()) {
            auto code = strToIPv4(serialized.as<const char *>(), &(_value.value), _statusCodes);
            if (code != _statusCodes.ok) return code;
            _value.isNull = false;
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool IPv4::serialize(const JsonVariant & serialized) const {
        if (_value.isNull) {
            serialized.clear();
            return true;
        }
        char szIP[16] = {};
        char * pos = szIP;
        uint32_t temp1 = _value.value;
        uint32_t temp2 = temp1 >> 8;
        uint32_t byte4 = temp1 - (temp2 << 8);
        temp1 = temp2 >> 8;
        uint32_t byte3 = temp2 - (temp1 << 8);
        temp2 = temp1 >> 8;
        uint32_t byte2 = temp1 - (temp2 << 8);
        temp1 = temp2 >> 8;
        uint32_t byte1 = temp2 - (temp1 << 8);
        pos = uint8ToStr(byte1, pos);
        *pos = '.';
        pos++;
        pos = uint8ToStr(byte2, pos);
        *pos = '.';
        pos++;
        pos = uint8ToStr(byte3, pos);
        *pos = '.';
        pos++;
        pos = uint8ToStr(byte4, pos);
        return serialized.set(szIP);
    }

}