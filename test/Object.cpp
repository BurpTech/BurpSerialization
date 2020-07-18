#include <unity.h>
#include "../src/BurpSerialization/Object.hpp"
#include "Object.hpp"

namespace Object {

    constexpr char fieldName[] = "field";
    constexpr char fieldOneName[] = "one";
    constexpr char fieldTwoName[] = "two";
    constexpr char fieldThreeName[] = "three";
    constexpr int invalidValue = 100;
    constexpr char validOneValue[] = "one value";
    constexpr char validTwoValue[] = "two value";
    constexpr char validThreeValue[] = "three value";

    enum : BurpStatus::Status::Code {
        ok,
        notPresent,
        wrongType,
        fieldOneNotPresent,
        fieldOneWrongType,
        fieldTwoNotPresent,
        fieldTwoWrongType,
        fieldThreeNotPresent,
        fieldThreeWrongType
    };

    class Field : public BurpSerialization::Field {
        public:
            struct StatusCodes {
                BurpStatus::Status::Code ok;
                BurpStatus::Status::Code notPresent;
                BurpStatus::Status::Code wrongType;
            };
            const StatusCodes statusCodes;
            const char * value;
            Field(const StatusCodes statusCodes) :
                statusCodes(statusCodes),
                value(nullptr)
            {}
            BurpStatus::Status::Code deserialize(const JsonVariant & serialized) override {
                value = nullptr;
                if (serialized.isNull()) {
                    return statusCodes.notPresent;
                }
                if (serialized.is<const char *>()) {
                    value = serialized.as<const char *>();
                    return statusCodes.ok;
                }
                return statusCodes.wrongType;
            }
            bool serialize(const JsonVariant & serialized) const override {
                return serialized.set(value);
            }
    };

    Field fieldOne({
        ok,
        fieldOneNotPresent,
        fieldOneWrongType
    });
    Field fieldTwo({
        ok,
        fieldTwoNotPresent,
        fieldTwoWrongType
    });
    Field fieldThree({
        ok,
        fieldThreeNotPresent,
        fieldThreeWrongType
    });
    using Object = BurpSerialization::Object<3>;
    Object object(Object::Entries({
        Object::Entry({fieldOneName, &fieldOne}),
        Object::Entry({fieldTwoName, &fieldTwo}),
        Object::Entry({fieldThreeName, &fieldThree})
    }), {
        ok,
        notPresent,
        wrongType
    });

    StaticJsonDocument<128> emptyDoc;
    StaticJsonDocument<128> invalidDoc;
    StaticJsonDocument<128> missingFieldTwoDoc;
    StaticJsonDocument<128> invalidFieldThreeDoc;
    StaticJsonDocument<128> validDoc;
    StaticJsonDocument<128> serializedDoc;
    StaticJsonDocument<1> tinyDoc;

    Module tests("Object", [](Describe & d) {
        d.before([]() {
            invalidDoc[fieldName] = invalidValue;
            missingFieldTwoDoc[fieldName][fieldOneName] = validOneValue;
            missingFieldTwoDoc[fieldName][fieldThreeName] = validThreeValue;
            invalidFieldThreeDoc[fieldName][fieldOneName] = validOneValue;
            invalidFieldThreeDoc[fieldName][fieldTwoName] = validTwoValue;
            invalidFieldThreeDoc[fieldName][fieldThreeName] = invalidValue;
            validDoc[fieldName][fieldOneName] = validOneValue;
            validDoc[fieldName][fieldTwoName] = validTwoValue;
            validDoc[fieldName][fieldThreeName] = validThreeValue;
        });

        d.beforeEach([]() {
            serializedDoc.clear();
            fieldOne.value = nullptr;
            fieldTwo.value = nullptr;
            fieldThree.value = nullptr;
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    const BurpStatus::Status::Code code = object.deserialize(emptyDoc[fieldName]);
                    TEST_ASSERT_FALSE(object.isPresent());
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    const BurpStatus::Status::Code code = object.deserialize(invalidDoc[fieldName]);
                    TEST_ASSERT_FALSE(object.isPresent());
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("when a sub field is not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    const BurpStatus::Status::Code code = object.deserialize(missingFieldTwoDoc[fieldName]);
                    TEST_ASSERT_FALSE(object.isPresent());
                    TEST_ASSERT_EQUAL(fieldTwoNotPresent, code);
                });
            });
            d.describe("when a sub field is invalid", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    const BurpStatus::Status::Code code = object.deserialize(invalidFieldThreeDoc[fieldName]);
                    TEST_ASSERT_FALSE(object.isPresent());
                    TEST_ASSERT_EQUAL(fieldThreeWrongType, code);
                });
            });
            d.describe("with a valid object", [](Describe & d) {
                d.it("should have the correct values", []() {
                    const BurpStatus::Status::Code code = object.deserialize(validDoc[fieldName]);
                    TEST_ASSERT_TRUE(object.isPresent());
                    TEST_ASSERT_EQUAL_STRING(validOneValue, fieldOne.value);
                    TEST_ASSERT_EQUAL_STRING(validTwoValue, fieldTwo.value);
                    TEST_ASSERT_EQUAL_STRING(validThreeValue, fieldThree.value);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    object.setPresent(true);
                    fieldOne.value = validOneValue;
                    fieldTwo.value = validTwoValue;
                    fieldThree.value = validThreeValue;
                    const bool success = object.serialize(tinyDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    object.setPresent(false);
                    const bool success = object.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    object.setPresent(true);
                    fieldOne.value = validOneValue;
                    fieldTwo.value = validTwoValue;
                    fieldThree.value = validThreeValue;
                    const bool success = object.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validOneValue, serializedDoc[fieldName][fieldOneName]);
                    TEST_ASSERT_EQUAL_STRING(validTwoValue, serializedDoc[fieldName][fieldTwoName]);
                    TEST_ASSERT_EQUAL_STRING(validThreeValue, serializedDoc[fieldName][fieldThreeName]);
                });
            });
        });
    });

}