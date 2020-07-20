#include <unity.h>
#include "../src/BurpSerialization/CStr.hpp"
#include "CStr.hpp"

namespace CStr {

    constexpr size_t length = 10;
    constexpr char fieldName[] = "field";
    constexpr int invalidCStr = 100;
    constexpr char emptyCStr[] = "";
    constexpr char longCStr[] = "01234567890";
    constexpr char validCStr[] = "0123456789";
    constexpr BurpSerialization::Value emptyValue = {.cstr=emptyCStr};
    constexpr BurpSerialization::Value nullValue = {.cstr=nullptr};
    constexpr BurpSerialization::Value validValue = {.cstr=validCStr};

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
            cstr.set(&emptyValue);
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and be null", []() {
                    auto code = cstr.deserialize(emptyDoc[fieldName]);
                    TEST_ASSERT_NULL(cstr.get());
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and be null", []() {
                    auto code = cstr.deserialize(invalidDoc[fieldName]);
                    TEST_ASSERT_NULL(cstr.get());
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with a value that is too long", [](Describe & d) {
                d.it("should fail and be null", []() {
                    auto code = cstr.deserialize(longDoc[fieldName]);
                    TEST_ASSERT_NULL(cstr.get());
                    TEST_ASSERT_EQUAL(tooLong, code);
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    auto code = cstr.deserialize(validDoc[fieldName]);
                    auto value = cstr.get();
                    TEST_ASSERT_NOT_NULL(value);
                    TEST_ASSERT_EQUAL_STRING(validCStr, value->cstr);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    cstr.set(&validValue);
                    auto success = cstr.serialize(tinyDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    serializedDoc[fieldName] = validCStr;
                    cstr.set(nullptr);
                    auto success = cstr.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a NULL value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    serializedDoc[fieldName] = validCStr;
                    cstr.set(&nullValue);
                    auto success = cstr.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    cstr.set(&validValue);
                    auto success = cstr.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validCStr, serializedDoc[fieldName]);
                });
            });
        });
    });

}