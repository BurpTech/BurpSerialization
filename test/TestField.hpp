#include "../src/BurpSerialization/Field.hpp"

class TestField : public BurpSerialization::Field {

    public:

        struct StatusCodes {
            BurpStatus::Status::Code ok;
            BurpStatus::Status::Code notPresent;
            BurpStatus::Status::Code wrongType;
        };

        TestField(const StatusCodes statusCodes);
        BurpStatus::Status::Code deserialize(BurpSerialization::Value & dest, const JsonVariant & src) override;
        bool serialize(const JsonVariant & dest, const BurpSerialization::Value & src) const override;

    private:

        const StatusCodes _statusCodes;

};