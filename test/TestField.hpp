#include "../src/BurpSerialization/Field.hpp"

class TestField : public BurpSerialization::Field {

    public:

        struct StatusCodes {
            BurpStatus::Status::Code ok;
            BurpStatus::Status::Code notPresent;
            BurpStatus::Status::Code wrongType;
        };

        TestField(const StatusCodes statusCodes, const char *& value);
        BurpStatus::Status::Code deserialize(const JsonVariant & src) const override;
        bool serialize(const JsonVariant & dest) const override;

    private:

        const StatusCodes _statusCodes;
        const char *& _value;

};