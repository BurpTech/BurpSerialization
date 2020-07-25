#pragma once

#include <ArduinoJson.h>
#include <BurpStatus.hpp>
#include "Value.hpp"

namespace BurpSerialization
{
    class Field
    {

    public:

        virtual BurpStatus::Status::Code deserialize(Value & dest, const JsonVariant & src) = 0;
        virtual bool serialize(const JsonVariant & dest, const Value & src) const = 0;

    };
    
}