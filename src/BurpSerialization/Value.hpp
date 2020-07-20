#pragma once

#include <inttypes.h>

namespace BurpSerialization
{

    union Value {
        const char * cstr;
        bool cbool;
        uint8_t uint8;
        uint32_t uint32;
        const uint8_t * uint8List;
        const Value ** valueList;
    };

}