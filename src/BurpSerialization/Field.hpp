#pragma once

#include <ArduinoJson.h>
#include <BurpStatus.hpp>

namespace BurpSerialization
{
    class Field
    {

    public:

        virtual BurpStatus::Status::Code deserialize(const JsonVariant & serialized) = 0;
        virtual bool serialize(const JsonVariant & serialized) const = 0;

    };
    
}