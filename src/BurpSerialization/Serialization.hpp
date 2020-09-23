#pragma once

#include "Field.hpp"

namespace BurpSerialization {

    class Serialization {

        public:

            Serialization(const Field & root);
            bool serialize(const JsonVariant & dest) const;
            BurpStatus::Status::Code deserialize(const JsonVariant & src);

        private:

            const Field & _root;

    };

}