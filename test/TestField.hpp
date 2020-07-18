#include "../src/BurpSerialization/TypedField.hpp"

class TestField : public BurpSerialization::TypedField<const char *> {

    public:

        struct StatusCodes {
            BurpStatus::Status::Code ok;
            BurpStatus::Status::Code notPresent;
            BurpStatus::Status::Code wrongType;
        };

        TestField(const StatusCodes statusCodes);
        BurpStatus::Status::Code deserialize(const JsonVariant & serialized) override;
        bool serialize(const JsonVariant & serialized) const override;
        const char * get() const override;
        void set(const char * value) override;

    private:

        const StatusCodes _statusCodes;
        const char * _value;

};