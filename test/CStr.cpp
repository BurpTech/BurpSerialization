#include <unity.h>
#include "../src/BurpSerialization/CStr.hpp"
#include "../src/BurpSerialization/Serialization.hpp"
#include "CStr.hpp"

namespace CStr {

    constexpr size_t docSize = 128;
    constexpr size_t minLength = 5;
    constexpr size_t maxLength = 10;
    constexpr char fieldName[] = "field";
    constexpr int invalidCStr = 100;
    constexpr char shortCStr[] = "0123";
    constexpr char longCStr[] = "01234567890";
    constexpr char minCStr[] = "01234";
    constexpr char maxCStr[] = "0123456789";

    class Serialization : public BurpSerialization::Serialization {

        public:

            enum : BurpStatus::Status::Code {
                ok,
                notPresent,
                wrongType,
                tooShort,
                tooLong
            };

            const char * cstr;

            Serialization() :
                BurpSerialization::Serialization(_cstr),
                _cstr(minLength, maxLength, {
                    ok,
                    notPresent,
                    wrongType,
                    tooShort,
                    tooLong
                }, cstr)
            {}

        private:

            const BurpSerialization::CStr _cstr;

    };

    Module tests("CStr", [](Describe & d) {
        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and be null", []() {
                    Serialization serialization;
                    StaticJsonDocument<1> doc;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_NULL(serialization.cstr);
                    TEST_ASSERT_EQUAL(Serialization::notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and be null", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = invalidCStr;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_NULL(serialization.cstr);
                    TEST_ASSERT_EQUAL(Serialization::wrongType, code);
                });
            });
            d.describe("with a value that is too short", [](Describe & d) {
                d.it("should fail and be null", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = shortCStr;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_NULL(serialization.cstr);
                    TEST_ASSERT_EQUAL(Serialization::tooShort, code);
                });
            });
            d.describe("with a value that is too long", [](Describe & d) {
                d.it("should fail and be null", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = longCStr;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_NULL(serialization.cstr);
                    TEST_ASSERT_EQUAL(Serialization::tooLong, code);
                });
            });
            d.describe("with a min length value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = minCStr;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_EQUAL_STRING(minCStr, serialization.cstr);
                    TEST_ASSERT_EQUAL(Serialization::ok, code);
                });
            });
            d.describe("with a max length value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = maxCStr;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_EQUAL_STRING(maxCStr, serialization.cstr);
                    TEST_ASSERT_EQUAL(Serialization::ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    Serialization serialization;
                    StaticJsonDocument<1> doc;
                    serialization.cstr = maxCStr;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = maxCStr;
                    serialization.cstr = nullptr;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(doc[fieldName].isNull());
                });
            });
            d.describe("with a value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    serialization.cstr = maxCStr;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_EQUAL_STRING(maxCStr, doc[fieldName]);
                });
            });
        });
    });

}