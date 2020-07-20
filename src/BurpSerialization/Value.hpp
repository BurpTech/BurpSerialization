#pragma once

#include <inttypes.h>

namespace BurpSerialization
{

    union Value {
        const char * cstr;
        bool cbool;
        int8_t int8;
        int16_t int16;
        int32_t int32;
        uint8_t uint8;
        uint16_t uint16;
        uint32_t uint32;
        const Value ** list;
    };

}