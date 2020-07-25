#pragma once

#include <inttypes.h>

namespace BurpSerialization
{

    struct Value {
        bool isNull;
        union {
            const char * cstr;
            bool cbool;
            uint8_t uint8;
            uint32_t uint32;
            uint8_t * uint8List;
            Value * valueList;
        };
    };

}