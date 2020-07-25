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

    BurpStatus::Status::Code strToUInt8(const char * src, uint32_t * dest, const char ** endPtr, IPv4::StatusCodes statusCodes) {
        uint16_t value = 0;
        *endPtr = src;
        while (**endPtr >= '0' && **endPtr <= '9') {
            value *= 10;
            value += (**endPtr) - '0';
            (*endPtr)++;
            if (value > 255) return statusCodes.outOfRange;
        }
        if (*endPtr == src) return statusCodes.missingField;
        *dest += value;
        return statusCodes.ok;
    }

    BurpStatus::Status::Code strToIPv4(const char * src, uint32_t * dest, IPv4::StatusCodes statusCodes) {
        *dest = 0;
        auto pos = src;
        for (uint8_t field = 0; field < 4; field++) {
            *dest *= 256;
            auto code = strToUInt8(pos, dest, &pos, statusCodes);
            if (code != statusCodes.ok) return code;
            if (field < 3) {
                if (*pos != '.') return statusCodes.invalidCharacter;
            } else {
                if (*pos != 0) return statusCodes.excessCharacters;
            }
            pos++;
        }
        return statusCodes.ok;
    }

    IPv4::IPv4(const StatusCodes statusCodes) :
        _statusCodes(statusCodes)
    {}

    BurpStatus::Status::Code IPv4::deserialize(Value & dest, const JsonVariant & src) const {
        dest.isNull = true;
        if (src.isNull()) {
            return _statusCodes.notPresent;
        }
        if (src.is<const char *>()) {
            auto code = strToIPv4(src.as<const char *>(), &(dest.uint32), _statusCodes);
            if (code != _statusCodes.ok) return code;
            dest.isNull = false;
            return _statusCodes.ok;
        }
        return _statusCodes.wrongType;
    }

    bool IPv4::serialize(const JsonVariant & dest, const Value & src) const {
        if (src.isNull) {
            dest.clear();
            return true;
        }
        char szIP[16] = {};
        char * ptr = szIP;
        uint32_t temp1 = src.uint32;
        uint32_t temp2 = temp1 >> 8;
        uint32_t byte4 = temp1 - (temp2 << 8);
        temp1 = temp2 >> 8;
        uint32_t byte3 = temp2 - (temp1 << 8);
        temp2 = temp1 >> 8;
        uint32_t byte2 = temp1 - (temp2 << 8);
        temp1 = temp2 >> 8;
        uint32_t byte1 = temp2 - (temp1 << 8);
        ptr = uint8ToStr(byte1, ptr);
        *ptr = '.';
        ptr++;
        ptr = uint8ToStr(byte2, ptr);
        *ptr = '.';
        ptr++;
        ptr = uint8ToStr(byte3, ptr);
        *ptr = '.';
        ptr++;
        ptr = uint8ToStr(byte4, ptr);
        return dest.set(szIP);
    }

}