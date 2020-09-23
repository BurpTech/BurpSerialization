#include <unity.h>
#include "../src/BurpSerialization/CStrMap.hpp"
#include "../src/BurpSerialization/Serialization.hpp"
#include "CStrMap.hpp"

namespace CStrMap {

    constexpr size_t docSize = 128;
    constexpr size_t choiceCount = 3;
    constexpr char fieldName[] = "field";
    constexpr char choiceOne[] = "one";
    constexpr uint8_t valueOne = 1;
    constexpr char choiceTwo[] = "two";
    constexpr uint8_t valueTwo = 2;
    constexpr char choiceThree[] = "three";
    constexpr uint8_t valueThree = 3;
    constexpr int invalidCStr = 100;
    constexpr char invalidChoice[] = "Four";
    constexpr uint8_t invalidValue = 4;
    using Map = BurpSerialization::CStrMap<uint8_t, choiceCount>;
    Map::Choices choices = {
        Map::Choice({choiceOne, valueOne}),
        Map::Choice({choiceTwo, valueTwo}),
        Map::Choice({choiceThree, valueThree})
    };

    class Serialization : public BurpSerialization::Serialization {

        public:

            enum : BurpStatus::Status::Code {
                ok,
                notPresent,
                wrongType,
                invalidChoice
            };

            Map::Value cstrMap;

            Serialization() :
                BurpSerialization::Serialization(_cstrMap),
                _cstrMap(choices, {
                    ok,
                    notPresent,
                    wrongType,
                    invalidChoice
                }, cstrMap)
            {}

        private:

            const Map _cstrMap;

    };

    Module tests("CStrMap", [](Describe & d) {
        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and be null", []() {
                    Serialization serialization;
                    StaticJsonDocument<1> doc;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.cstrMap.isNull);
                    TEST_ASSERT_EQUAL(Serialization::notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and be null", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = invalidCStr;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.cstrMap.isNull);
                    TEST_ASSERT_EQUAL(Serialization::wrongType, code);
                });
            });
            d.describe("with an invalid choice", [](Describe & d) {
                d.it("should fail and be null", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = invalidChoice;
                    auto code = serialization.deserialize(doc[fieldName]);
                    TEST_ASSERT_TRUE(serialization.cstrMap.isNull);
                    TEST_ASSERT_EQUAL(Serialization::invalidChoice, code);
                });
            });
            d.describe("with a valid value", [](Describe & d) {
                for (auto choice : choices) {
                    d.describe(choice.key, [=](Describe & d) {
                        d.it("should not fail and have the correct value", [=]() {
                            Serialization serialization;
                            StaticJsonDocument<docSize> doc;
                            doc[fieldName] = choice.key;
                            auto code = serialization.deserialize(doc[fieldName]);
                            TEST_ASSERT_FALSE(serialization.cstrMap.isNull);
                            TEST_ASSERT_EQUAL(choice.value, serialization.cstrMap.value);
                            TEST_ASSERT_EQUAL(Serialization::ok, code);
                        });
                    });
                }
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    Serialization serialization;
                    StaticJsonDocument<1> doc;
                    serialization.cstrMap.value = 2;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    serialization.cstrMap.value = invalidValue;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                    TEST_ASSERT_TRUE(doc[fieldName].isNull());
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    Serialization serialization;
                    StaticJsonDocument<docSize> doc;
                    doc[fieldName] = invalidChoice;
                    serialization.cstrMap.isNull = true;
                    auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(doc[fieldName].isNull());
                });
            });
            d.describe("with a value", [](Describe & d) {
                for (auto choice : choices) {
                    d.describe(choice.key, [=](Describe & d) {
                        d.it("should set the value in the JSON document", [=]() {
                            Serialization serialization;
                            StaticJsonDocument<docSize> doc;
                            serialization.cstrMap.value = choice.value;
                            auto success = serialization.serialize(doc[fieldName].to<JsonVariant>());
                            TEST_ASSERT_TRUE(success);
                            TEST_ASSERT_EQUAL_STRING(choice.key, doc[fieldName]);
                        });
                    });
                }
            });
        });
    });

}