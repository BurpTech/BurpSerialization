#include <unity.h>
#include "../src/BurpSerialization/PWMLevels.hpp"
#include "../src/BurpSerialization/Serialization.hpp"
#include "PWMLevels.hpp"

namespace PWMLevels {

    constexpr size_t docSize = 10000;
    constexpr char fieldName[] = "field";
    constexpr char invalidLevel[] = "hello";
    BurpSerialization::PWMLevels::List longList = {};
    BurpSerialization::PWMLevels::List fullList = {};
    BurpSerialization::PWMLevels::List shortList = {};

    class Serialization : public BurpSerialization::Serialization {

        public:

            enum : BurpStatus::Status::Code {
                ok,
                notPresent,
                wrongType,
                tooLong,
                tooShort,
                levelZero,
                levelNotIncreasing,
                levelNotPresent,
                levelWrongType
            };

            BurpSerialization::PWMLevels::Value pwmLevels;

            Serialization() :
                BurpSerialization::Serialization(_pwmLevels),
                _pwmLevels({
                    ok,
                    notPresent,
                    wrongType,
                    tooLong,
                    tooShort,
                    levelZero,
                    levelNotIncreasing,
                    levelNotPresent,
                    levelWrongType
                }, pwmLevels)
            {}

        private:

            const BurpSerialization::PWMLevels _pwmLevels;

    };

    Module tests("PWMLevels", [](Describe & d) {
        d.before([]() {
            for (size_t index = 0; index < BurpSerialization::PWMLevels::maxLevels + 1; index++) {
                longList[index] = (index % 255) + 1 ;
            }
            for (size_t index = 0; index < BurpSerialization::PWMLevels::maxLevels; index++) {
                fullList[index] = (index % 255) + 1;
            }
            for (size_t index = 0; index < BurpSerialization::PWMLevels::maxLevels - 1; index++) {
                shortList[index] = (index % 255) + 1;
            }
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.pwmLevels.isNull);
                    TEST_ASSERT_EQUAL(Serialization::notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    doc[fieldName] = invalidLevel;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.pwmLevels.isNull);
                    TEST_ASSERT_EQUAL(Serialization::wrongType, code);
                });
            });
            d.describe("when the array is too long", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    for (size_t index = 0; index < BurpSerialization::PWMLevels::maxLevels + 1; index++) {
                        doc[fieldName].add(index + 1);
                    }
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.pwmLevels.isNull);
                    TEST_ASSERT_EQUAL(Serialization::tooLong, code);
                });
            });
            d.describe("when the array is too short", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    doc[fieldName].to<JsonArray>();
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.pwmLevels.isNull);
                    TEST_ASSERT_EQUAL(Serialization::tooShort, code);
                });
            });
            d.describe("when a level is not increasing", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    doc[fieldName].add(1);
                    doc[fieldName].add(1);
                    doc[fieldName].add(2);
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.pwmLevels.isNull);
                    TEST_ASSERT_EQUAL(Serialization::levelNotIncreasing, code);
                });
            });
            d.describe("when a level is zero", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    doc[fieldName].add(1);
                    doc[fieldName].add(0);
                    doc[fieldName].add(2);
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.pwmLevels.isNull);
                    TEST_ASSERT_EQUAL(Serialization::levelZero, code);
                });
            });
            d.describe("when a level is missing", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    doc[fieldName].add(1);
                    doc[fieldName].add(nullptr);
                    doc[fieldName].add(2);
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.pwmLevels.isNull);
                    TEST_ASSERT_EQUAL(Serialization::levelNotPresent, code);
                });
            });
            d.describe("when a level is invalid", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    doc[fieldName].add(1);
                    doc[fieldName].add(invalidLevel);
                    doc[fieldName].add(2);
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.pwmLevels.isNull);
                    TEST_ASSERT_EQUAL(Serialization::levelWrongType, code);
                });
            });
            d.describe("with a full array", [](Describe & d) {
                d.it("should have the correct values", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    for (size_t index = 0; index < BurpSerialization::PWMLevels::maxLevels; index++) {
                        doc[fieldName].add(index + 1);
                    }
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_FALSE(serialization.pwmLevels.isNull);
                    TEST_ASSERT_EQUAL(BurpSerialization::PWMLevels::maxLevels, serialization.pwmLevels.length);
                    for (size_t index = 0; index < fullList.size(); index++) {
                        TEST_ASSERT_EQUAL(fullList[index], serialization.pwmLevels.list[index]);
                    }
                    TEST_ASSERT_EQUAL(Serialization::ok, code);
                });
            });
            d.describe("with a short array", [](Describe & d) {
                d.it("should have the correct values", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    for (size_t index = 0; index < BurpSerialization::PWMLevels::maxLevels - 1; index++) {
                        doc[fieldName].add(index + 1);
                    }
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_FALSE(serialization.pwmLevels.isNull);
                    TEST_ASSERT_EQUAL(BurpSerialization::PWMLevels::maxLevels - 1, serialization.pwmLevels.length);
                    for (size_t index = 0; index < shortList.size(); index++) {
                        TEST_ASSERT_EQUAL(shortList[index], serialization.pwmLevels.list[index]);
                    }
                    TEST_ASSERT_EQUAL(Serialization::ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(1);
                    serialization.pwmLevels.list = fullList;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    serialization.pwmLevels.isNull = true;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(doc[fieldName].isNull());
                });
            });
            d.describe("with a long list of values", [](Describe & d) {
                d.it("should fail", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    serialization.pwmLevels.list = longList;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("with a full list of values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    serialization.pwmLevels.list = fullList;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    for (size_t index = 0; index < BurpSerialization::PWMLevels::maxLevels; index++) {
                        TEST_ASSERT_EQUAL(fullList[index], doc[fieldName][index]);
                    }
                    TEST_ASSERT_TRUE(doc[fieldName][BurpSerialization::PWMLevels::maxLevels].isNull());
                });
            });
            d.describe("with a short list of values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    Serialization serialization;
                    DynamicJsonDocument doc(docSize);
                    serialization.pwmLevels.list = shortList;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    for (size_t index = 0; index < BurpSerialization::PWMLevels::maxLevels - 1; index++) {
                        TEST_ASSERT_EQUAL(shortList[index], doc[fieldName][index]);
                    }
                    TEST_ASSERT_TRUE(doc[fieldName][BurpSerialization::PWMLevels::maxLevels - 1].isNull());
                });
            });
        });
    });

}