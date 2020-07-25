#include <unity.h>
#include "../src/BurpSerialization/IPv4.hpp"
#include "IPv4.hpp"

namespace IPv4 {

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

    StaticJsonDocument<128> emptyDoc;
    StaticJsonDocument<128> wrongTypeDoc;
    StaticJsonDocument<128> invalidCharacterDoc;
    StaticJsonDocument<128> outOfRangeDoc;
    StaticJsonDocument<128> missingByteDoc;
    StaticJsonDocument<128> excessCharactersDoc;
    StaticJsonDocument<128> validDoc;
    StaticJsonDocument<128> serializedDoc;
    StaticJsonDocument<1> tinyDoc;

    Module tests("IPv4", [](Describe & d) {
        d.before([]() {
            wrongTypeDoc[fieldName] = wrongTypeIPv4;
            invalidCharacterDoc[fieldName] = invalidCharacterIPv4;
            outOfRangeDoc[fieldName] = outOfRangeIPv4;
            missingByteDoc[fieldName] = missingByteIPv4;
            excessCharactersDoc[fieldName] = excessCharactersIPv4;
            validDoc[fieldName] = validIPv4;
        });

        d.beforeEach([]() {
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, emptyDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with a value of the wrong type", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, wrongTypeDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with an unparseable value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, invalidCharacterDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(invalidCharacter, code);
                });
            });
            d.describe("with an out of range value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, outOfRangeDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(outOfRange, code);
                });
            });
            d.describe("with a missing byte", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, missingByteDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(missingByte, code);
                });
            });
            d.describe("with too many bytes", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, excessCharactersDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(excessCharacters, code);
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    BurpSerialization::Value value;
                    auto code = ipv4.deserialize(value, validDoc[fieldName]);
                    TEST_ASSERT_FALSE(value.isNull);
                    TEST_ASSERT_EQUAL(validUInt32, value.uint32);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    auto success = ipv4.serialize(tinyDoc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    serializedDoc[fieldName] = true;
                    auto success = ipv4.serialize(serializedDoc[fieldName].to<JsonVariant>(), nullValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    auto success = ipv4.serialize(serializedDoc[fieldName].to<JsonVariant>(), validValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validIPv4, serializedDoc[fieldName]);
                });
            });
        });
    });

}