#include <unity.h>
#include "../src/BurpSerialization/Array.hpp"
#include "TestField.hpp"
#include "Array.hpp"

namespace Array {

    constexpr size_t length = 5;
    constexpr char fieldName[] = "field";
    constexpr int invalidCStr = 100;
    constexpr char validCStr1[] = "value1";
    constexpr char validCStr2[] = "value2";
    constexpr char validCStr3[] = "value3";
    constexpr char validCStr4[] = "value4";
    constexpr char validCStr5[] = "value5";
    constexpr char validCStr6[] = "value6";
    constexpr BurpSerialization::Value validValue1 = {.cstr=validCStr1};
    constexpr BurpSerialization::Value validValue2 = {.cstr=validCStr2};
    constexpr BurpSerialization::Value validValue3 = {.cstr=validCStr3};
    constexpr BurpSerialization::Value validValue4 = {.cstr=validCStr4};
    constexpr BurpSerialization::Value validValue5 = {.cstr=validCStr5};

    enum : BurpStatus::Status::Code {
        ok,
        notPresent,
        wrongType,
        tooLong,
        fieldWrongType
    };

    TestField field({
        ok,
        ok,
        fieldWrongType
    });
    using Array = BurpSerialization::Array<length>;
    Array array(field, {
        ok,
        notPresent,
        wrongType,
        tooLong
    });

    StaticJsonDocument<256> emptyDoc;
    StaticJsonDocument<256> invalidDoc;
    StaticJsonDocument<256> invalidFieldDoc;
    StaticJsonDocument<256> longDoc;
    StaticJsonDocument<256> validDoc;
    StaticJsonDocument<256> shortDoc;
    StaticJsonDocument<256> serializedDoc;
    StaticJsonDocument<1> tinyDoc;

    Array::List fullList = {
        &validValue1,
        &validValue2,
        &validValue3,
        &validValue4,
        &validValue5
    };
    BurpSerialization::Value fullValue = {.list=fullList.data()};

    Array::List shortList = {
        &validValue1,
        &validValue2,
        &validValue3,
        &validValue4
    };
    BurpSerialization::Value shortValue = {.list=shortList.data()};

    Module tests("Array", [](Describe & d) {
        d.before([]() {
            invalidDoc[fieldName] = invalidCStr;
            invalidFieldDoc[fieldName].add(validCStr1);
            invalidFieldDoc[fieldName].add(invalidCStr);
            invalidFieldDoc[fieldName].add(validCStr3);
            longDoc[fieldName].add(validCStr1);
            longDoc[fieldName].add(validCStr2);
            longDoc[fieldName].add(validCStr3);
            longDoc[fieldName].add(validCStr4);
            longDoc[fieldName].add(validCStr5);
            longDoc[fieldName].add(validCStr6);
            validDoc[fieldName].add(validCStr1);
            validDoc[fieldName].add(validCStr2);
            validDoc[fieldName].add(validCStr3);
            validDoc[fieldName].add(validCStr4);
            validDoc[fieldName].add(validCStr5);
            shortDoc[fieldName].add(validCStr1);
            shortDoc[fieldName].add(validCStr2);
            shortDoc[fieldName].add(validCStr3);
            shortDoc[fieldName].add(validCStr4);
        });

        d.beforeEach([]() {
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = array.deserialize(emptyDoc[fieldName]);
                    TEST_ASSERT_NULL(array.get());
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = array.deserialize(invalidDoc[fieldName]);
                    TEST_ASSERT_NULL(array.get());
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("when the array is too long", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = array.deserialize(longDoc[fieldName]);
                    TEST_ASSERT_NULL(array.get());
                    TEST_ASSERT_EQUAL(tooLong, code);
                });
            });
            d.describe("when an array entry is invalid", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = array.deserialize(invalidFieldDoc[fieldName]);
                    TEST_ASSERT_NULL(array.get());
                    TEST_ASSERT_EQUAL(fieldWrongType, code);
                });
            });
            d.describe("with a full array", [](Describe & d) {
                d.it("should have the correct values", []() {
                    auto code = array.deserialize(validDoc[fieldName]);
                    auto value = array.get();
                    TEST_ASSERT_NOT_NULL(value);
                    auto list = value->list;
                    for (size_t index = 0; index < length; index++) {
                        TEST_ASSERT_EQUAL_STRING(fullList[index]->cstr, list[index]->cstr);
                    }
                });
            });
            d.describe("with a short array", [](Describe & d) {
                d.it("should have the correct values", []() {
                    auto code = array.deserialize(shortDoc[fieldName]);
                    auto value = array.get();
                    TEST_ASSERT_NOT_NULL(value);
                    auto list = value->list;
                    for (size_t index = 0; index < length; index++) {
                        TEST_ASSERT_EQUAL_STRING(shortList[index]->cstr, list[index]->cstr);
                    }
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    array.set(&fullValue);
                    auto success = array.serialize(tinyDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    array.set(nullptr);
                    auto success = array.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a full list of values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    array.set(&fullValue);
                    auto success = array.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validCStr1, serializedDoc[fieldName][0]);
                    TEST_ASSERT_EQUAL_STRING(validCStr2, serializedDoc[fieldName][1]);
                    TEST_ASSERT_EQUAL_STRING(validCStr3, serializedDoc[fieldName][2]);
                    TEST_ASSERT_EQUAL_STRING(validCStr4, serializedDoc[fieldName][3]);
                    TEST_ASSERT_EQUAL_STRING(validCStr5, serializedDoc[fieldName][4]);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName][5].isNull());
                });
            });
            d.describe("with a short list of values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    array.set(&shortValue);
                    auto success = array.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validCStr1, serializedDoc[fieldName][0]);
                    TEST_ASSERT_EQUAL_STRING(validCStr2, serializedDoc[fieldName][1]);
                    TEST_ASSERT_EQUAL_STRING(validCStr3, serializedDoc[fieldName][2]);
                    TEST_ASSERT_EQUAL_STRING(validCStr4, serializedDoc[fieldName][3]);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName][4].isNull());
                });
            });
        });
    });

}