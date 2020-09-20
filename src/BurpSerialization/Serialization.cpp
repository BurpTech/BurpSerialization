#include "Serialization.hpp"

namespace BurpSerialization {

    Serialization::Serialization(const Field & root) :
        _root(root)
    {}

    bool Serialization::serialize(const JsonVariant & dest) const {
        return _root.serialize(dest);
    }

    BurpStatus::Status::Code Serialization::deserialize(const JsonVariant & src) {
        return _root.deserialize(src);
    }

}