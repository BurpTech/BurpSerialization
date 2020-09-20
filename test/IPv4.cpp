#include <unity.h>
#include "../src/BurpSerialization/IPv4.hpp"
#include "../src/BurpSerialization/Serialization.hpp"
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

    class Serialization : public BurpSerialization::Serialization {

        public:

            enum : BurpStatus::Status::Code {
                ok,
                notPresent,
                wrongType,
                invalidCharacter,
                outOfRange,
                missingByte,
                excessCharacters
            };

            BurpSerialization::IPv4::Value ipv4;

            Serialization() :
                BurpSerialization::Serialization(_ipv4),
                _ipv4({
                    ok,
                    notPresent,
                    wrongType,
                    invalidCharacter,
                    outOfRange,
                    missingByte,
                    excessCharacters
                }, ipv4)
            {}

        private:

            const BurpSerialization::IPv4 _ipv4;

    };

    Module tests("IPv4", [](Describe & d) {
        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<1> doc;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.ipv4.isNull);
                    TEST_ASSERT_EQUAL(Serialization::notPresent, code);
                });
            });
            d.describe("with a value of the wrong type", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = wrongTypeIPv4;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.ipv4.isNull);
                    TEST_ASSERT_EQUAL(Serialization::wrongType, code);
                });
            });
            d.describe("with an unparseable value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = invalidCharacterIPv4;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.ipv4.isNull);
                    TEST_ASSERT_EQUAL(Serialization::invalidCharacter, code);
                });
            });
            d.describe("with an out of range value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = outOfRangeIPv4;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.ipv4.isNull);
                    TEST_ASSERT_EQUAL(Serialization::outOfRange, code);
                });
            });
            d.describe("with a missing byte", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = missingByteIPv4;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.ipv4.isNull);
                    TEST_ASSERT_EQUAL(Serialization::missingByte, code);
                });
            });
            d.describe("with too many bytes", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = excessCharactersIPv4;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.ipv4.isNull);
                    TEST_ASSERT_EQUAL(Serialization::excessCharacters, code);
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = validIPv4;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_FALSE(serialization.ipv4.isNull);
                    TEST_ASSERT_EQUAL(validUInt32, serialization.ipv4.value);
                    TEST_ASSERT_EQUAL(Serialization::ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    Serialization serialization;
                    StaticJsonDocument<1> doc;
                    serialization.ipv4.isNull = false;
                    serialization.ipv4.value = validUInt32;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = true;
                    serialization.ipv4.isNull = true;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(doc[fieldName].isNull());
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    serialization.ipv4.isNull = false;
                    serialization.ipv4.value = validUInt32;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(validIPv4, doc[fieldName]);
                });
            });
        });
    });

}