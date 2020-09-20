#include <unity.h>
#include "../src/BurpSerialization/Object.hpp"
#include "../src/BurpSerialization/Serialization.hpp"
#include "TestField.hpp"
#include "Object.hpp"

namespace Object {

    constexpr size_t docSize = 128;
    constexpr size_t entryCount = 3;
    constexpr char fieldName[] = "field";
    constexpr char fieldOneName[] = "one";
    constexpr char fieldTwoName[] = "two";
    constexpr char fieldThreeName[] = "three";
    constexpr int invalidCStr = 100;
    constexpr char validOneCStr[] = "one value";
    constexpr char validTwoCStr[] = "two value";
    constexpr char validThreeCStr[] = "three value";

    class Serialization : public BurpSerialization::Serialization {

        public:

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

            struct {
                bool isNull;
                const char * fieldOne;
                const char * fieldTwo;
                const char * fieldThree;
            } obj;

            Serialization() :
                BurpSerialization::Serialization(_obj),
                _fieldOne({
                    ok,
                    fieldOneNotPresent,
                    fieldOneWrongType
                }, obj.fieldOne),
                _fieldTwo({
                    ok,
                    fieldTwoNotPresent,
                    fieldTwoWrongType
                }, obj.fieldTwo),
                _fieldThree({
                    ok,
                    fieldThreeNotPresent,
                    fieldThreeWrongType
                }, obj.fieldThree),
                _obj({
                    Object::Entry({fieldOneName, &_fieldOne}),
                    Object::Entry({fieldTwoName, &_fieldTwo}),
                    Object::Entry({fieldThreeName, &_fieldThree})
                }, {
                    ok,
                    notPresent,
                    wrongType
                }, obj.isNull)
            {}

        private:

            using Object = BurpSerialization::Object<entryCount>;

            const TestField _fieldOne;
            const TestField _fieldTwo;
            const TestField _fieldThree;
            const Object _obj;

    };

    Module tests("Object", [](Describe & d) {
        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<1> doc;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.obj.isNull);
                    TEST_ASSERT_EQUAL(Serialization::notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = invalidCStr;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.obj.isNull);
                    TEST_ASSERT_EQUAL(Serialization::wrongType, code);
                });
            });
            d.describe("when a sub field is not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName][fieldOneName] = validOneCStr;
                    doc[fieldName][fieldThreeName] = validThreeCStr;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_FALSE(serialization.obj.isNull);
                    TEST_ASSERT_EQUAL_STRING(validOneCStr, serialization.obj.fieldOne);
                    TEST_ASSERT_NULL(serialization.obj.fieldTwo);
                    TEST_ASSERT_EQUAL_STRING(validThreeCStr, serialization.obj.fieldThree);
                    TEST_ASSERT_EQUAL(Serialization::fieldTwoNotPresent, code);
                });
            });
            d.describe("when a sub field is invalid", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName][fieldOneName] = validOneCStr;
                    doc[fieldName][fieldTwoName] = validTwoCStr;
                    doc[fieldName][fieldThreeName] = invalidCStr;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_FALSE(serialization.obj.isNull);
                    TEST_ASSERT_EQUAL(Serialization::fieldThreeWrongType, code);
                });
            });
            d.describe("with a valid object", [](Describe & d) {
                d.it("should have the correct values", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName][fieldOneName] = validOneCStr;
                    doc[fieldName][fieldTwoName] = validTwoCStr;
                    doc[fieldName][fieldThreeName] = validThreeCStr;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_FALSE(serialization.obj.isNull);
                    TEST_ASSERT_EQUAL_STRING(validOneCStr, serialization.obj.fieldOne);
                    TEST_ASSERT_EQUAL_STRING(validTwoCStr, serialization.obj.fieldTwo);
                    TEST_ASSERT_EQUAL_STRING(validThreeCStr, serialization.obj.fieldThree);
                    TEST_ASSERT_EQUAL(Serialization::ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    Serialization serialization;
                    StaticJsonDocument<1> doc;
                    serialization.obj.isNull = false;
                    serialization.obj.fieldOne = validOneCStr;
                    serialization.obj.fieldTwo = validTwoCStr;
                    serialization.obj.fieldThree = validThreeCStr;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    serialization.obj.isNull = true;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(doc[fieldName].isNull());
                });
            });
            d.describe("with values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    serialization.obj.isNull = false;
                    serialization.obj.fieldOne = validOneCStr;
                    serialization.obj.fieldTwo = validTwoCStr;
                    serialization.obj.fieldThree = validThreeCStr;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validOneCStr, doc[fieldName][fieldOneName]);
                    TEST_ASSERT_EQUAL_STRING(validTwoCStr, doc[fieldName][fieldTwoName]);
                    TEST_ASSERT_EQUAL_STRING(validThreeCStr, doc[fieldName][fieldThreeName]);
                });
            });
        });
    });

}