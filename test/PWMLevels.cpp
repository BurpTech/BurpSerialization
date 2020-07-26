#include <unity.h>
#include "../src/BurpSerialization/PWMLevels.hpp"
#include "PWMLevels.hpp"

namespace PWMLevels {

    constexpr size_t docSize = 10000;
    constexpr char fieldName[] = "field";
    constexpr char invalidLevel[] = "hello";
    constexpr BurpSerialization::Value nullValue = {true};

    BurpSerialization::PWMLevels::List longList = {};
    BurpSerialization::Value longValue = {false, {.uint8List=longList.data()}};

    BurpSerialization::PWMLevels::List fullList = {};
    BurpSerialization::Value fullValue = {false, {.uint8List=fullList.data()}};

    BurpSerialization::PWMLevels::List shortList = {};
    BurpSerialization::Value shortValue = {false, {.uint8List=shortList.data()}};

    enum : BurpStatus::Status::Code {
        ok,
        notPresent,
        wrongType,
        tooLong,
        levelZero,
        levelNotIncreasing,
        levelNotPresent,
        levelWrongType
    };

    BurpSerialization::PWMLevels pwmLevels({
        ok,
        notPresent,
        wrongType,
        tooLong,
        levelZero,
        levelNotIncreasing,
        levelNotPresent,
        levelWrongType
    });


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
                    DynamicJsonDocument doc(docSize);
                    BurpSerialization::PWMLevels::List list;
                    BurpSerialization::Value value = {true, {.uint8List=list.data()}};
                    auto code = pwmLevels.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    DynamicJsonDocument doc(docSize);
                    doc[fieldName] = invalidLevel;
                    BurpSerialization::PWMLevels::List list;
                    BurpSerialization::Value value = {true, {.uint8List=list.data()}};
                    auto code = pwmLevels.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("when the array is too long", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    DynamicJsonDocument doc(docSize);
                    for (size_t index = 0; index < BurpSerialization::PWMLevels::maxLevels + 1; index++) {
                        doc[fieldName].add(index + 1);
                    }
                    BurpSerialization::PWMLevels::List list;
                    BurpSerialization::Value value = {true, {.uint8List=list.data()}};
                    auto code = pwmLevels.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(tooLong, code);
                });
            });
            d.describe("when a level is not increasing", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    DynamicJsonDocument doc(docSize);
                    doc[fieldName].add(1);
                    doc[fieldName].add(1);
                    doc[fieldName].add(2);
                    BurpSerialization::PWMLevels::List list;
                    BurpSerialization::Value value = {true, {.uint8List=list.data()}};
                    auto code = pwmLevels.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(levelNotIncreasing, code);
                });
            });
            d.describe("when a level is zero", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    DynamicJsonDocument doc(docSize);
                    doc[fieldName].add(1);
                    doc[fieldName].add(0);
                    doc[fieldName].add(2);
                    BurpSerialization::PWMLevels::List list;
                    BurpSerialization::Value value = {true, {.uint8List=list.data()}};
                    auto code = pwmLevels.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(levelZero, code);
                });
            });
            d.describe("when a level is missing", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    DynamicJsonDocument doc(docSize);
                    doc[fieldName].add(1);
                    doc[fieldName].add(nullptr);
                    doc[fieldName].add(2);
                    BurpSerialization::PWMLevels::List list;
                    BurpSerialization::Value value = {true, {.uint8List=list.data()}};
                    auto code = pwmLevels.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(levelNotPresent, code);
                });
            });
            d.describe("when a level is invalid", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    DynamicJsonDocument doc(docSize);
                    doc[fieldName].add(1);
                    doc[fieldName].add(invalidLevel);
                    doc[fieldName].add(2);
                    BurpSerialization::PWMLevels::List list;
                    BurpSerialization::Value value = {true, {.uint8List=list.data()}};
                    auto code = pwmLevels.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(levelWrongType, code);
                });
            });
            d.describe("with a full array", [](Describe & d) {
                d.it("should have the correct values", []() {
                    DynamicJsonDocument doc(docSize);
                    for (size_t index = 0; index < BurpSerialization::PWMLevels::maxLevels; index++) {
                        doc[fieldName].add(index + 1);
                    }
                    BurpSerialization::PWMLevels::List list;
                    BurpSerialization::Value value = {true, {.uint8List=list.data()}};
                    auto code = pwmLevels.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_FALSE(value.isNull);
                    for (size_t index = 0; index < fullList.size(); index++) {
                        TEST_ASSERT_EQUAL(fullList[index], list[index]);
                    }
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
            d.describe("with a short array", [](Describe & d) {
                d.it("should have the correct values", []() {
                    DynamicJsonDocument doc(docSize);
                    for (size_t index = 0; index < BurpSerialization::PWMLevels::maxLevels - 1; index++) {
                        doc[fieldName].add(index + 1);
                    }
                    BurpSerialization::PWMLevels::List list;
                    BurpSerialization::Value value = {true, {.uint8List=list.data()}};
                    auto code = pwmLevels.deserialize(value, doc[fieldName]);
                    TEST_ASSERT_FALSE(value.isNull);
                    for (size_t index = 0; index < shortList.size(); index++) {
                        TEST_ASSERT_EQUAL(shortList[index], list[index]);
                    }
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    DynamicJsonDocument doc(1);
                    auto success = pwmLevels.serialize(doc[fieldName].to<JsonVariant>(), fullValue);
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    DynamicJsonDocument doc(docSize);
                    auto success = pwmLevels.serialize(doc[fieldName].to<JsonVariant>(), nullValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(doc[fieldName].isNull());
                });
            });
            d.describe("with a long list of values", [](Describe & d) {
                d.it("should fail", []() {
                    DynamicJsonDocument doc(docSize);
                    auto success = pwmLevels.serialize(doc[fieldName].to<JsonVariant>(), longValue);
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("with a full list of values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    DynamicJsonDocument doc(docSize);
                    auto success = pwmLevels.serialize(doc[fieldName].to<JsonVariant>(), fullValue);
                    TEST_ASSERT_TRUE(success);
                    for (size_t index = 0; index < BurpSerialization::PWMLevels::maxLevels; index++) {
                        TEST_ASSERT_EQUAL(fullList[index], doc[fieldName][index]);
                    }
                    TEST_ASSERT_TRUE(doc[fieldName][BurpSerialization::PWMLevels::maxLevels].isNull());
                });
            });
            d.describe("with a short list of values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    DynamicJsonDocument doc(docSize);
                    auto success = pwmLevels.serialize(doc[fieldName].to<JsonVariant>(), shortValue);
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