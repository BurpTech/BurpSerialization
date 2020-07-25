#include <unity.h>
#include "../src/BurpSerialization/CStr.hpp"
#include "CStr.hpp"

namespace CStr {

    constexpr size_t length = 10;
    constexpr char fieldName[] = "field";
    constexpr int invalidCStr = 100;
    constexpr char longCStr[] = "01234567890";
    constexpr char validCStr[] = "0123456789";
    constexpr BurpSerialization::Value nullValue = {true};
    constexpr BurpSerialization::Value nullCStrValue = {false, {.cstr=nullptr}};
    constexpr BurpSerialization::Value validValue = {false, {.cstr=validCStr}};

    enum : BurpStatus::Status::Code {
        ok,
        notPresent,
        wrongType,
        tooLong
    };

    BurpSerialization::CStr cstr(length, {
        ok,
        notPresent,
        wrongType,
        tooLong
    });

    StaticJsonDocument<128> emptyDoc;
    StaticJsonDocument<128> invalidDoc;
    StaticJsonDocument<128> longDoc;
    StaticJsonDocument<128> validDoc;
    StaticJsonDocument<128> serializedDoc;
    StaticJsonDocument<1> tinyDoc;

    Module tests("CStr", [](Describe & d) {
        d.before([]() {
            invalidDoc[fieldName] = invalidCStr;
            longDoc[fieldName] = longCStr;
            validDoc[fieldName] = validCStr;
        });

        d.beforeEach([]() {
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and be null", []() {
                    BurpSerialization::Value value;
                    auto code = cstr.deserialize(value, emptyDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and be null", []() {
                    BurpSerialization::Value value;
                    auto code = cstr.deserialize(value, invalidDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with a value that is too long", [](Describe & d) {
                d.it("should fail and be null", []() {
                    BurpSerialization::Value value;
                    auto code = cstr.deserialize(value, longDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(tooLong, code);
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    BurpSerialization::Value value;
                    auto code = cstr.deserialize(value, validDoc[fieldName]);
                    TEST_ASSERT_FALSE(value.isNull);
                    TEST_ASSERT_EQUAL_STRING(validCStr, value.cstr);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    auto success = cstr.serialize(tinyDoc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    serializedDoc[fieldName] = validCStr;
                    auto success = cstr.serialize(serializedDoc[fieldName].to<JsonVariant>(), nullValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a NULL value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    serializedDoc[fieldName] = validCStr;
                    auto success = cstr.serialize(serializedDoc[fieldName].to<JsonVariant>(), nullCStrValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    auto success = cstr.serialize(serializedDoc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validCStr, serializedDoc[fieldName]);
                });
            });
        });
    });

}