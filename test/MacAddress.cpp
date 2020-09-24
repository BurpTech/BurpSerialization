#include <unity.h>
#include "../src/BurpSerialization/MacAddress.hpp"
#include "../src/BurpSerialization/Serialization.hpp"
#include "MacAddress.hpp"

namespace MacAddress {

    constexpr size_t docSize = 128;
    constexpr char fieldName[] = "field";
    constexpr int wrongTypeMacAddress = 100;
    constexpr char invalidCharacterMacAddress[] = "10hello";
    constexpr char invalidSeparatorMacAddress[] = "10:ed-23";
    constexpr char outOfRangeMacAddress[] = "12:34:45a:67:89:ab";
    constexpr char missingByteMacAddress[] = "12:34::78:9a:bc";
    constexpr char excessCharactersMacAddress[] = "12:34:56:78:9a:bc:de";
    constexpr char validMacAddressHyphen[] = "12-34-5-78-9A-bC";
    constexpr char validMacAddressColon[] = "12:34:5:78:9A:bC";
    constexpr char serializedMacAddress[] = "12:34:05:78:9A:BC";
    constexpr uint8_t validArray[BurpSerialization::MAC_ADDRESS_BYTE_COUNT] = {
        0x12,
        0x34,
        0x5,
        0x78,
        0x9a,
        0xbc
    };

    class Serialization : public BurpSerialization::Serialization {

        public:

            enum : BurpStatus::Status::Code {
                ok,
                notPresent,
                wrongType,
                invalidCharacter,
                invalidSeparator,
                outOfRange,
                missingByte,
                excessCharacters
            };

            BurpSerialization::MacAddress::Value macAddress;

            Serialization() :
                BurpSerialization::Serialization(_macAddress),
                _macAddress({
                    ok,
                    notPresent,
                    wrongType,
                    invalidCharacter,
                    invalidSeparator,
                    outOfRange,
                    missingByte,
                    excessCharacters
                }, macAddress)
            {}

        private:

            const BurpSerialization::MacAddress _macAddress;

    };

    Module tests("MacAddress", [](Describe & d) {
        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<1> doc;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.macAddress.isNull);
                    TEST_ASSERT_EQUAL(Serialization::notPresent, code);
                });
            });
            d.describe("with a value of the wrong type", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = wrongTypeMacAddress;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.macAddress.isNull);
                    TEST_ASSERT_EQUAL(Serialization::wrongType, code);
                });
            });
            d.describe("with an unparseable value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = invalidCharacterMacAddress;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.macAddress.isNull);
                    TEST_ASSERT_EQUAL(Serialization::invalidCharacter, code);
                });
            });
            d.describe("with an inconsistent separator", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = invalidSeparatorMacAddress;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.macAddress.isNull);
                    TEST_ASSERT_EQUAL(Serialization::invalidSeparator, code);
                });
            });
            d.describe("with an out of range value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = outOfRangeMacAddress;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.macAddress.isNull);
                    TEST_ASSERT_EQUAL(Serialization::outOfRange, code);
                });
            });
            d.describe("with a missing byte", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = missingByteMacAddress;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.macAddress.isNull);
                    TEST_ASSERT_EQUAL(Serialization::missingByte, code);
                });
            });
            d.describe("with too many bytes", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = excessCharactersMacAddress;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.macAddress.isNull);
                    TEST_ASSERT_EQUAL(Serialization::excessCharacters, code);
                });
            });
            d.describe("with a valid value using hyphens", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = validMacAddressHyphen;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_FALSE(serialization.macAddress.isNull);
                    TEST_ASSERT_EQUAL_UINT8_ARRAY(validArray, serialization.macAddress.value, BurpSerialization::MAC_ADDRESS_BYTE_COUNT);
                    TEST_ASSERT_EQUAL(Serialization::ok, code);
                });
            });
            d.describe("with a valid value using colons", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = validMacAddressColon;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_FALSE(serialization.macAddress.isNull);
                    TEST_ASSERT_EQUAL_UINT8_ARRAY(validArray, serialization.macAddress.value, BurpSerialization::MAC_ADDRESS_BYTE_COUNT);
                    TEST_ASSERT_EQUAL(Serialization::ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    Serialization serialization;
                    StaticJsonDocument<1> doc;
                    memcpy(serialization.macAddress.value, validArray, BurpSerialization::MAC_ADDRESS_BYTE_COUNT);
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = true;
                    serialization.macAddress.isNull = true;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(doc[fieldName].isNull());
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    memcpy(serialization.macAddress.value, validArray, BurpSerialization::MAC_ADDRESS_BYTE_COUNT);
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(serializedMacAddress, doc[fieldName]);
                });
            });
        });
    });

}