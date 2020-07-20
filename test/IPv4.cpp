#include <unity.h>
#include "../src/BurpSerialization/IPv4.hpp"
#include "IPv4.hpp"

namespace IPv4 {

    constexpr char fieldName[] = "field";
    constexpr int wrongTypeIPv4 = 100;
    constexpr char parseErrorIPv4[] = "hello";
    constexpr char outOfRangeIPv4[] = "255.256.255.255";
    constexpr char missingByteIPv4[] = "255..255.255";
    constexpr char tooFewBytesIPv4[] = "255.255.255";
    constexpr char tooManyBytesIPv4[] = "255.255.255.255.255";
    constexpr char validIPv4[] = "255.255.255.255";
    constexpr BurpSerialization::Value validValue = {.uint32=UINT32_MAX};

    enum : BurpStatus::Status::Code {
        ok,
        notPresent,
        wrongType,
        parseError,
        outOfRange,
        missingByte,
        tooFewBytes,
        tooManyBytes
    };

    BurpSerialization::IPv4 ipv4({
        ok,
        notPresent,
        wrongType,
        parseError,
        outOfRange,
        missingByte,
        tooFewBytes,
        tooManyBytes
    });

    StaticJsonDocument<128> emptyDoc;
    StaticJsonDocument<128> wrongTypeDoc;
    StaticJsonDocument<128> parseErrorDoc;
    StaticJsonDocument<128> outOfRangeDoc;
    StaticJsonDocument<128> missingByteDoc;
    StaticJsonDocument<128> tooFewBytesDoc;
    StaticJsonDocument<128> tooManyBytesDoc;
    StaticJsonDocument<128> validDoc;
    StaticJsonDocument<128> serializedDoc;
    StaticJsonDocument<1> tinyDoc;

    Module tests("IPv4", [](Describe & d) {
        d.before([]() {
            wrongTypeDoc[fieldName] = wrongTypeIPv4;
            parseErrorDoc[fieldName] = parseErrorIPv4;
            outOfRangeDoc[fieldName] = outOfRangeIPv4;
            missingByteDoc[fieldName] = missingByteIPv4;
            tooFewBytesDoc[fieldName] = tooFewBytesIPv4;
            tooManyBytesDoc[fieldName] = tooManyBytesIPv4;
            validDoc[fieldName] = validIPv4;
        });

        d.beforeEach([]() {
            ipv4.set(&validValue);
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = ipv4.deserialize(emptyDoc[fieldName]);
                    TEST_ASSERT_NULL(ipv4.get());
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with a value of the wrong type", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = ipv4.deserialize(wrongTypeDoc[fieldName]);
                    TEST_ASSERT_NULL(ipv4.get());
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with an unparseable value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = ipv4.deserialize(parseErrorDoc[fieldName]);
                    TEST_ASSERT_NULL(ipv4.get());
                    TEST_ASSERT_EQUAL(parseError, code);
                });
            });
            d.describe("with an out of range value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = ipv4.deserialize(outOfRangeDoc[fieldName]);
                    TEST_ASSERT_NULL(ipv4.get());
                    TEST_ASSERT_EQUAL(outOfRange, code);
                });
            });
            d.describe("with a missing byte", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = ipv4.deserialize(missingByteDoc[fieldName]);
                    TEST_ASSERT_NULL(ipv4.get());
                    TEST_ASSERT_EQUAL(missingByte, code);
                });
            });
            d.describe("with too few bytes", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = ipv4.deserialize(tooFewBytesDoc[fieldName]);
                    TEST_ASSERT_NULL(ipv4.get());
                    TEST_ASSERT_EQUAL(tooFewBytes, code);
                });
            });
            d.describe("with too many bytes", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = ipv4.deserialize(tooManyBytesDoc[fieldName]);
                    TEST_ASSERT_NULL(ipv4.get());
                    TEST_ASSERT_EQUAL(tooManyBytes, code);
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    ipv4.set(nullptr);
                    auto code = ipv4.deserialize(validDoc[fieldName]);
                    auto value = ipv4.get();
                    TEST_ASSERT_NOT_NULL(value);
                    TEST_ASSERT_EQUAL(UINT32_MAX, value->uint32);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    ipv4.set(&validValue);
                    auto success = ipv4.serialize(tinyDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    serializedDoc[fieldName] = true;
                    ipv4.set(nullptr);
                    auto success = ipv4.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    ipv4.set(&validValue);
                    auto success = ipv4.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL(validIPv4, serializedDoc[fieldName]);
                });
            });
        });
    });

}