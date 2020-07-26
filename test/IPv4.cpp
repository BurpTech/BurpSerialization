#include <unity.h>
#include "../src/BurpSerialization/IPv4.hpp"
#include "IPv4.hpp"

namespace IPv4 {

    constexpr size_t docSize = 128;
    constexpr char fieldName[] = "field";
    constexpr int wrongTypeIPv4 = 100;
    constexpr char invalidCharacterIPv4[] = "100hello";
    constexpr char outOfRangeIPv4[] = "255.256.255.255";
    constexpr char missingByteIPv4[] = "255..255.255";
    constexpr char excessCharactersIPv4[] = "255.255.255.255.255";
    constexpr char validIPv4[] = "10.0.100.1";
    constexpr uint32_t validUInt32 = ((((((10 * 256) + 0) * 256) + 100) * 256) + 1);
    constexpr BurpSerialization::Value nullValue = {true};
    constexpr BurpSerialization::Value validValue = {false, {.uint32=validUInt32}};

    enum : BurpStatus::Status::Code {
        ok,
        notPresent,
        wrongType,
        invalidCharacter,
        outOfRange,
        missingByte,
        excessCharacters
    };

    BurpSerialization::IPv4 ipv4({
        ok,
        notPresent,
        wrongType,
        invalidCharacter,
        outOfRange,
        missingByte,
        excessCharacters
    });

    Module tests("IPv4", [](Describe & d) {
        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    StaticJsonDocument<1> doc;
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with a value of the wrong type", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = wrongTypeIPv4;
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with an unparseable value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = invalidCharacterIPv4;
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(invalidCharacter, code);
                });
            });
            d.describe("with an out of range value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = outOfRangeIPv4;
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(outOfRange, code);
                });
            });
            d.describe("with a missing byte", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = missingByteIPv4;
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(missingByte, code);
                });
            });
            d.describe("with too many bytes", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = excessCharactersIPv4;
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(excessCharacters, code);
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = validIPv4;
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_FALSE(value.isNull);
                    TEST_ASSERT_EQUAL(validUInt32, value.uint32);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    StaticJsonDocument<1> doc;
                    auto success = ipv4.serialize(doc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = true;
                    auto success = ipv4.serialize(doc[fieldName].to<JsonVariant>(), nullValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(doc[fieldName].isNull());
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    StaticJsonDocument<docSize> doc;
                    auto success = ipv4.serialize(doc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validIPv4, doc[fieldName]);
                });
            });
        });
    });

}