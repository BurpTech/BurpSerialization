#include <unity.h>
#include "../src/BurpSerialization/CStr.hpp"
#include "CStr.hpp"

namespace CStr {

    constexpr size_t docSize = 128;
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

    Module tests("CStr", [](Describe & d) {
        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and be null", []() {
                    StaticJsonDocument<1> doc;
                    BurpSerialization::Value value;
                    auto code = cstr.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and be null", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = invalidCStr;
                    BurpSerialization::Value value;
                    auto code = cstr.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with a value that is too long", [](Describe & d) {
                d.it("should fail and be null", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = longCStr;
                    BurpSerialization::Value value;
                    auto code = cstr.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(tooLong, code);
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = validCStr;
                    BurpSerialization::Value value;
                    auto code = cstr.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_FALSE(value.isNull);
                    TEST_ASSERT_EQUAL_STRING(validCStr, value.cstr);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    StaticJsonDocument<1> doc;
                    auto success = cstr.serialize(doc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = validCStr;
                    auto success = cstr.serialize(doc[fieldName].to<JsonVariant>(), nullValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(doc[fieldName].isNull());
                });
            });
            d.describe("with a NULL value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = validCStr;
                    auto success = cstr.serialize(doc[fieldName].to<JsonVariant>(), nullCStrValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(doc[fieldName].isNull());
                });
            });
            d.describe("with a value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    StaticJsonDocument<docSize> doc;
                    auto success = cstr.serialize(doc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validCStr, doc[fieldName]);
                });
            });
        });
    });

}