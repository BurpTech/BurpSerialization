#pragma once

#include "Field.hpp"

namespace BurpSerialization
{
    template <class Type>
    class TypedField : public Field
    {

    public:

        virtual Type get() const = 0;
        virtual void set(const Type value) = 0;

    };
    
}