#pragma once

#include <array>
#include "Value.hpp"

namespace BurpSerialization
{

    template <size_t count>
    using ValueList = std::array<const Value *, count>;

}