#include <unity.h>
#include "../src/BurpSerialization/Array.hpp"
#include "TestField.hpp"
#include "Array.hpp"

namespace Array {

    constexpr size_t length = 5;
    constexpr char fieldName[] = "field";
    constexpr int invalidValue = 100;
    constexpr char validValue1[] = "value1";
    constexpr char validValue2[] = "value2";
    constexpr char validValue3[] = "value3";
    constexpr char validValue4[] = "value4";
    constexpr char validValue5[] = "value5";
    constexpr char validValue6[] = "value6";

    enum : BurpStatus::Status::Code {
        ok,
        notPresent,
        wrongType,
        tooLong,
        zeroEntry,
        fieldWrongType
    };

    TestField field({
        ok,
        ok,
        fieldWrongType
    });
    using Array = BurpSerialization::Array<const char *, length>;
    Array array(field, {
        ok,
        notPresent,
        wrongType,
        tooLong,
        zeroEntry
    });

    StaticJsonDocument<256> emptyDoc;
    StaticJsonDocument<256> invalidDoc;
    StaticJsonDocument<256> missingFieldDoc;
    StaticJsonDocument<256> invalidFieldDoc;
    StaticJsonDocument<256> longDoc;
    StaticJsonDocument<256> validDoc;
    StaticJsonDocument<256> shortDoc;
    StaticJsonDocument<256> serializedDoc;
    StaticJsonDocument<1> tinyDoc;

    Array::List longList = {
        validValue1,
        validValue2,
        validValue3,
        validValue4,
        validValue5,
        validValue6
    };

    Array::List fullList = {
        validValue1,
        validValue2,
        validValue3,
        validValue4,
        validValue5
    };

    Array::List shortList = {
        validValue1,
        validValue2,
        validValue3,
        validValue4
    };

    Module tests("Array", [](Describe & d) {
        d.before([]() {
            invalidDoc[fieldName] = invalidValue;
            missingFieldDoc[fieldName].add(validValue1);
            missingFieldDoc[fieldName].add(nullptr);
            missingFieldDoc[fieldName].add(validValue3);
            invalidFieldDoc[fieldName].add(validValue1);
            invalidFieldDoc[fieldName].add(invalidValue);
            invalidFieldDoc[fieldName].add(validValue3);
            longDoc[fieldName].add(validValue1);
            longDoc[fieldName].add(validValue2);
            longDoc[fieldName].add(validValue3);
            longDoc[fieldName].add(validValue4);
            longDoc[fieldName].add(validValue5);
            longDoc[fieldName].add(validValue6);
            validDoc[fieldName].add(validValue1);
            validDoc[fieldName].add(validValue2);
            validDoc[fieldName].add(validValue3);
            validDoc[fieldName].add(validValue4);
            validDoc[fieldName].add(validValue5);
            shortDoc[fieldName].add(validValue1);
            shortDoc[fieldName].add(validValue2);
            shortDoc[fieldName].add(validValue3);
            shortDoc[fieldName].add(validValue4);
        });

        d.beforeEach([]() {
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    const BurpStatus::Status::Code code = array.deserialize(emptyDoc[fieldName]);
                    TEST_ASSERT_FALSE(array.isPresent());
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    const BurpStatus::Status::Code code = array.deserialize(invalidDoc[fieldName]);
                    TEST_ASSERT_FALSE(array.isPresent());
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("when the array is too long", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    const BurpStatus::Status::Code code = array.deserialize(longDoc[fieldName]);
                    TEST_ASSERT_FALSE(array.isPresent());
                    TEST_ASSERT_EQUAL(tooLong, code);
                });
            });
            d.describe("when an array entry is NULL", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    const BurpStatus::Status::Code code = array.deserialize(missingFieldDoc[fieldName]);
                    TEST_ASSERT_FALSE(array.isPresent());
                    TEST_ASSERT_EQUAL(zeroEntry, code);
                });
            });
            d.describe("when an array entry is invalid", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    const BurpStatus::Status::Code code = array.deserialize(invalidFieldDoc[fieldName]);
                    TEST_ASSERT_FALSE(array.isPresent());
                    TEST_ASSERT_EQUAL(fieldWrongType, code);
                });
            });
            d.describe("with a full array", [](Describe & d) {
                d.it("should have the correct values", []() {
                    const BurpStatus::Status::Code code = array.deserialize(validDoc[fieldName]);
                    TEST_ASSERT_TRUE(array.isPresent());
                    const auto list = array.get();
                    for (size_t index = 0; index < length; index++) {
                        TEST_ASSERT_EQUAL_STRING(fullList[index], list[index]);
                    }
                });
            });
            d.describe("with a short array", [](Describe & d) {
                d.it("should have the correct values", []() {
                    const BurpStatus::Status::Code code = array.deserialize(shortDoc[fieldName]);
                    TEST_ASSERT_TRUE(array.isPresent());
                    const auto list = array.get();
                    for (size_t index = 0; index < length; index++) {
                        TEST_ASSERT_EQUAL_STRING(shortList[index], list[index]);
                    }
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    array.set(fullList);
                    const bool success = array.serialize(tinyDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    array.setNotPresent();
                    const bool success = array.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a long list of values", [](Describe & d) {
                d.it("should fail", []() {
                    array.set(longList);
                    const bool success = array.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("with a full list of values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    array.set(fullList);
                    const bool success = array.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validValue1, serializedDoc[fieldName][0]);
                    TEST_ASSERT_EQUAL_STRING(validValue2, serializedDoc[fieldName][1]);
                    TEST_ASSERT_EQUAL_STRING(validValue3, serializedDoc[fieldName][2]);
                    TEST_ASSERT_EQUAL_STRING(validValue4, serializedDoc[fieldName][3]);
                    TEST_ASSERT_EQUAL_STRING(validValue5, serializedDoc[fieldName][4]);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName][5].isNull());
                });
            });
            d.describe("with a short list of values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    array.set(shortList);
                    const bool success = array.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validValue1, serializedDoc[fieldName][0]);
                    TEST_ASSERT_EQUAL_STRING(validValue2, serializedDoc[fieldName][1]);
                    TEST_ASSERT_EQUAL_STRING(validValue3, serializedDoc[fieldName][2]);
                    TEST_ASSERT_EQUAL_STRING(validValue4, serializedDoc[fieldName][3]);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName][4].isNull());
                });
            });
        });
    });

}