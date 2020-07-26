#pragma once

#include <inttypes.h>

#define _BURP_SERIALIZATION_OPERATORS(TYPE, FIELD)\
    operator TYPE () const {return FIELD;};\
    Value & operator = (TYPE value) {isNull = false; FIELD = value; return *this;}


namespace BurpSerialization
{

    struct Value {
        bool isNull;
        union {
            Value * valueList;
            bool cbool;
            uint8_t uint8;
            uint16_t uint16;
            uint32_t uint32;
            int8_t int8;
            int16_t int16;
            int32_t int32;
            const char * cstr;
            uint8_t * uint8List;
        };
        _BURP_SERIALIZATION_OPERATORS(bool, cbool);
        _BURP_SERIALIZATION_OPERATORS(uint8_t, uint8);
        _BURP_SERIALIZATION_OPERATORS(uint16_t, uint16);
        _BURP_SERIALIZATION_OPERATORS(uint32_t, uint32);
        _BURP_SERIALIZATION_OPERATORS(int8_t, int8);
        _BURP_SERIALIZATION_OPERATORS(int16_t, int16);
        _BURP_SERIALIZATION_OPERATORS(int32_t, int32);
        _BURP_SERIALIZATION_OPERATORS(const char *, cstr);
    };

}