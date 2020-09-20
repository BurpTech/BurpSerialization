#pragma once

#include <ArduinoJson.h>
#include <BurpStatus.hpp>

namespace BurpSerialization
{
    class Field
    {

    public:

        virtual BurpStatus::Status::Code deserialize(const JsonVariant & src) const = 0;
        virtual bool serialize(const JsonVariant & dest) const = 0;

    };
    
}