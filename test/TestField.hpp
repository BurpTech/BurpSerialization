#include "../src/BurpSerialization/Field.hpp"

class TestField : public BurpSerialization::Field {

    public:

        struct StatusCodes {
            BurpStatus::Status::Code ok;
            BurpStatus::Status::Code notPresent;
            BurpStatus::Status::Code wrongType;
        };

        TestField(const StatusCodes statusCodes);
        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) override;
        bool serialize(const JsonVariant & serialized) const override;
        const BurpSerialization::Value * get() const override;
        void set(const BurpSerialization::Value * value) override;

    private:

        const StatusCodes _statusCodes;
        BurpSerialization::Value _value;

};