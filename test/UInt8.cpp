#include <unity.h>
#include "../src/BurpSerialization/UInt8.hpp"
#include "UInt8.hpp"

namespace UInt8 {

    constexpr char fieldName[] = "field";
    constexpr char invalidUInt8[] = "hello";
    constexpr uint8_t validUInt8 = UINT8_MAX;
    constexpr BurpSerialization::Value nullValue = {true};
    constexpr BurpSerialization::Value validValue = {false, {.uint8=validUInt8}};

    enum : BurpStatus::Status::Code {
        ok,
        notPresent,
        wrongType
    };

    BurpSerialization::UInt8 uint8({
        ok,
        notPresent,
        wrongType
    });

    StaticJsonDocument<128> emptyDoc;
    StaticJsonDocument<128> invalidDoc;
    StaticJsonDocument<128> validDoc;
    StaticJsonDocument<128> serializedDoc;
    StaticJsonDocument<1> tinyDoc;

    Module tests("UInt8", [](Describe & d) {
        d.before([]() {
            invalidDoc[fieldName] = invalidUInt8;
            validDoc[fieldName] = validUInt8;
        });

        d.beforeEach([]() {
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    BurpSerialization::Value value;
                    auto code = uint8.deserialize(value, emptyDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    BurpSerialization::Value value;
                    auto code = uint8.deserialize(value, invalidDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    BurpSerialization::Value value;
                    auto code = uint8.deserialize(value, validDoc[fieldName]);
                    TEST_ASSERT_FALSE(value.isNull);
                    TEST_ASSERT_EQUAL(validUInt8, value.uint8);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    auto success = uint8.serialize(tinyDoc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    serializedDoc[fieldName] = validUInt8;
                    auto success = uint8.serialize(serializedDoc[fieldName].to<JsonVariant>(), nullValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    auto success = uint8.serialize(serializedDoc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL(validUInt8, serializedDoc[fieldName]);
                });
            });
        });
    });

}