#include <unity.h>
#include "../src/BurpSerialization/Object.hpp"
#include "TestField.hpp"
#include "Object.hpp"

namespace Object {

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

    StaticJsonDocument<128> emptyDoc;
    StaticJsonDocument<128> invalidDoc;
    StaticJsonDocument<128> missingFieldTwoDoc;
    StaticJsonDocument<128> invalidFieldThreeDoc;
    StaticJsonDocument<128> validDoc;
    StaticJsonDocument<128> serializedDoc;
    StaticJsonDocument<1> tinyDoc;

    Module tests("Object", [](Describe & d) {
        d.before([]() {
            invalidDoc[fieldName] = invalidCStr;
            missingFieldTwoDoc[fieldName][fieldOneName] = validOneCStr;
            missingFieldTwoDoc[fieldName][fieldThreeName] = validThreeCStr;
            invalidFieldThreeDoc[fieldName][fieldOneName] = validOneCStr;
            invalidFieldThreeDoc[fieldName][fieldTwoName] = validTwoCStr;
            invalidFieldThreeDoc[fieldName][fieldThreeName] = invalidCStr;
            validDoc[fieldName][fieldOneName] = validOneCStr;
            validDoc[fieldName][fieldTwoName] = validTwoCStr;
            validDoc[fieldName][fieldThreeName] = validThreeCStr;
        });

        d.beforeEach([]() {
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Object::List list;
                    BurpSerialization::Value value = {true, {.valueList=list.data()}};
                    auto code = object.deserialize(value, emptyDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Object::List list;
                    BurpSerialization::Value value = {true, {.valueList=list.data()}};
                    auto code = object.deserialize(value, invalidDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("when a sub field is not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Object::List list;
                    BurpSerialization::Value value = {true, {.valueList=list.data()}};
                    auto code = object.deserialize(value, missingFieldTwoDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(fieldTwoNotPresent, code);
                });
            });
            d.describe("when a sub field is invalid", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Object::List list;
                    BurpSerialization::Value value = {true, {.valueList=list.data()}};
                    auto code = object.deserialize(value, invalidFieldThreeDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(fieldThreeWrongType, code);
                });
            });
            d.describe("with a valid object", [](Describe & d) {
                d.it("should have the correct values", []() {
                    Object::List list;
                    BurpSerialization::Value value = {true, {.valueList=list.data()}};
                    auto code = object.deserialize(value, validDoc[fieldName]);
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
                    auto success = object.serialize(tinyDoc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    auto success = object.serialize(serializedDoc[fieldName].to<JsonVariant>(), nullValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    auto success = object.serialize(serializedDoc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validOneCStr, serializedDoc[fieldName][fieldOneName]);
                    TEST_ASSERT_EQUAL_STRING(validTwoCStr, serializedDoc[fieldName][fieldTwoName]);
                    TEST_ASSERT_EQUAL_STRING(validThreeCStr, serializedDoc[fieldName][fieldThreeName]);
                });
            });
        });
    });

}