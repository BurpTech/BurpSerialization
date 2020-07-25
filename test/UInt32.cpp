#include <unity.h>
#include "../src/BurpSerialization/UInt32.hpp"
#include "UInt32.hpp"

namespace UInt32 {

    constexpr char fieldName[] = "field";
    constexpr char invalidUInt32[] = "hello";
    constexpr uint32_t validUInt32 = UINT32_MAX;
    constexpr BurpSerialization::Value nullValue = {true};
    constexpr BurpSerialization::Value validValue = {false, {.uint32=validUInt32}};

    enum : BurpStatus::Status::Code {
        ok,
        notPresent,
        wrongType
    };

    BurpSerialization::UInt32 uint32({
        ok,
        notPresent,
        wrongType
    });

    StaticJsonDocument<128> emptyDoc;
    StaticJsonDocument<128> invalidDoc;
    StaticJsonDocument<128> validDoc;
    StaticJsonDocument<128> serializedDoc;
    StaticJsonDocument<1> tinyDoc;

    Module tests("UInt32", [](Describe & d) {
        d.before([]() {
            invalidDoc[fieldName] = invalidUInt32;
            validDoc[fieldName] = validUInt32;
        });

        d.beforeEach([]() {
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    BurpSerialization::Value value;
                    auto code = uint32.deserialize(value, emptyDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    BurpSerialization::Value value;
                    auto code = uint32.deserialize(value, invalidDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    BurpSerialization::Value value;
                    auto code = uint32.deserialize(value, validDoc[fieldName]);
                    TEST_ASSERT_FALSE(value.isNull);
                    TEST_ASSERT_EQUAL(validUInt32, value.uint32);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    auto success = uint32.serialize(tinyDoc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    serializedDoc[fieldName] = validUInt32;
                    auto success = uint32.serialize(serializedDoc[fieldName].to<JsonVariant>(), nullValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    auto success = uint32.serialize(serializedDoc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL(validUInt32, serializedDoc[fieldName]);
                });
            });
        });
    });

}