#include <unity.h>
#include "../src/BurpSerialization/CStr.hpp"
#include "CStr.hpp"

namespace CStr {

    constexpr size_t length = 10;
    constexpr char fieldName[] = "field";
    constexpr char emptyValue[] = "";
    constexpr int invalidValue = 100;
    constexpr char longValue[] = "01234567890";
    constexpr char validValue[] = "0123456789";

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
            invalidDoc[fieldName] = invalidValue;
            longDoc[fieldName] = longValue;
            validDoc[fieldName] = validValue;
        });

        d.beforeEach([]() {
            cstr.set(emptyValue);
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and be null", []() {
                    const BurpStatus::Status::Code code = cstr.deserialize(emptyDoc[fieldName]);
                    TEST_ASSERT_NULL(cstr.get());
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and be null", []() {
                    const BurpStatus::Status::Code code = cstr.deserialize(invalidDoc[fieldName]);
                    TEST_ASSERT_NULL(cstr.get());
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with a value that is too long", [](Describe & d) {
                d.it("should fail and be null", []() {
                    const BurpStatus::Status::Code code = cstr.deserialize(longDoc[fieldName]);
                    TEST_ASSERT_NULL(cstr.get());
                    TEST_ASSERT_EQUAL(tooLong, code);
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    const BurpStatus::Status::Code code = cstr.deserialize(validDoc[fieldName]);
                    TEST_ASSERT_EQUAL_STRING(validValue, cstr.get());
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    cstr.set(validValue);
                    const bool success = cstr.serialize(tinyDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    serializedDoc[fieldName] = validValue;
                    cstr.set(nullptr);
                    const bool success = cstr.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    cstr.set(validValue);
                    const bool success = cstr.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validValue, serializedDoc[fieldName]);
                });
            });
        });
    });

}