#include <unity.h>
#include "../src/BurpSerialization/Object.hpp"
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
    constexpr BurpSerialization::Value nullValue = {true};
    constexpr BurpSerialization::Value validOneValue = {false, {.cstr=validOneCStr}};
    constexpr BurpSerialization::Value validTwoValue = {false, {.cstr=validTwoCStr}};
    constexpr BurpSerialization::Value validThreeValue = {false, {.cstr=validThreeCStr}};

    using Object = BurpSerialization::Object<entryCount>;
    Object::List validList = {
        validOneValue,
        validTwoValue,
        validThreeValue
    };
    BurpSerialization::Value validValue = {false, {.valueList=validList.data()}};

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

    TestField fieldOne({
        ok,
        fieldOneNotPresent,
        fieldOneWrongType
    });
    TestField fieldTwo({
        ok,
        fieldTwoNotPresent,
        fieldTwoWrongType
    });
    TestField fieldThree({
        ok,
        fieldThreeNotPresent,
        fieldThreeWrongType
    });
    Object object(Object::Entries({
        Object::Entry({fieldOneName, &fieldOne}),
        Object::Entry({fieldTwoName, &fieldTwo}),
        Object::Entry({fieldThreeName, &fieldThree})
    }), {
        ok,
        notPresent,
        wrongType
    });

    Module tests("Object", [](Describe & d) {
        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    StaticJsonDocument<1> doc;
                    Object::List list;
                    BurpSerialization::Value value = {true, {.valueList=list.data()}};
                    auto code = object.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = invalidCStr;
                    Object::List list;
                    BurpSerialization::Value value = {true, {.valueList=list.data()}};
                    auto code = object.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("when a sub field is not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName][fieldOneName] = validOneCStr;
                    doc[fieldName][fieldThreeName] = validThreeCStr;
                    Object::List list;
                    BurpSerialization::Value value = {true, {.valueList=list.data()}};
                    auto code = object.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(fieldTwoNotPresent, code);
                });
            });
            d.describe("when a sub field is invalid", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName][fieldOneName] = validOneCStr;
                    doc[fieldName][fieldTwoName] = validTwoCStr;
                    doc[fieldName][fieldThreeName] = invalidCStr;
                    Object::List list;
                    BurpSerialization::Value value = {true, {.valueList=list.data()}};
                    auto code = object.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(fieldThreeWrongType, code);
                });
            });
            d.describe("with a valid object", [](Describe & d) {
                d.it("should have the correct values", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName][fieldOneName] = validOneCStr;
                    doc[fieldName][fieldTwoName] = validTwoCStr;
                    doc[fieldName][fieldThreeName] = validThreeCStr;
                    Object::List list;
                    BurpSerialization::Value value = {true, {.valueList=list.data()}};
                    auto code = object.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_FALSE(value.isNull);
                    auto valueOne = list[0];
                    TEST_ASSERT_FALSE(valueOne.isNull);
                    auto valueTwo = list[1];
                    TEST_ASSERT_FALSE(valueTwo.isNull);
                    auto valueThree = list[2];
                    TEST_ASSERT_FALSE(valueThree.isNull);
                    TEST_ASSERT_EQUAL_STRING(validOneCStr, valueOne.cstr);
                    TEST_ASSERT_EQUAL_STRING(validTwoCStr, valueTwo.cstr);
                    TEST_ASSERT_EQUAL_STRING(validThreeCStr, valueThree.cstr);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    StaticJsonDocument<1> doc;
                    auto success = object.serialize(doc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    StaticJsonDocument<docSize> doc;
                    auto success = object.serialize(doc[fieldName].to<JsonVariant>(), nullValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(doc[fieldName].isNull());
                });
            });
            d.describe("with values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    StaticJsonDocument<docSize> doc;
                    auto success = object.serialize(doc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validOneCStr, doc[fieldName][fieldOneName]);
                    TEST_ASSERT_EQUAL_STRING(validTwoCStr, doc[fieldName][fieldTwoName]);
                    TEST_ASSERT_EQUAL_STRING(validThreeCStr, doc[fieldName][fieldThreeName]);
                });
            });
        });
    });

}