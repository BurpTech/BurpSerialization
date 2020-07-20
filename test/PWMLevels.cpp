#include <unity.h>
#include "../src/BurpSerialization/PWMLevels.hpp"
#include "PWMLevels.hpp"

namespace PWMLevels {

    constexpr char fieldName[] = "field";
    constexpr char invalidLevel[] = "hello";

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

    StaticJsonDocument<10000> emptyDoc;
    StaticJsonDocument<10000> invalidDoc;
    StaticJsonDocument<10000> zeroFieldDoc;
    StaticJsonDocument<10000> notIncreasingFieldDoc;
    StaticJsonDocument<10000> missingFieldDoc;
    StaticJsonDocument<10000> invalidFieldDoc;
    StaticJsonDocument<10000> longDoc;
    StaticJsonDocument<10000> fullDoc;
    StaticJsonDocument<10000> shortDoc;
    StaticJsonDocument<10000> serializedDoc;
    StaticJsonDocument<1> tinyDoc;

    BurpSerialization::PWMLevels::List fullList = {};
    BurpSerialization::Value fullValue = {.uint8List=fullList.data()};

    BurpSerialization::PWMLevels::List shortList = {};
    BurpSerialization::Value shortValue = {.uint8List=shortList.data()};

    Module tests("PWMLevels", [](Describe & d) {
        d.before([]() {
            invalidDoc[fieldName] = invalidLevel;
            notIncreasingFieldDoc[fieldName].add(1);
            notIncreasingFieldDoc[fieldName].add(1);
            notIncreasingFieldDoc[fieldName].add(2);
            zeroFieldDoc[fieldName].add(1);
            zeroFieldDoc[fieldName].add(0);
            zeroFieldDoc[fieldName].add(2);
            missingFieldDoc[fieldName].add(1);
            missingFieldDoc[fieldName].add(nullptr);
            missingFieldDoc[fieldName].add(2);
            invalidFieldDoc[fieldName].add(1);
            invalidFieldDoc[fieldName].add(invalidLevel);
            invalidFieldDoc[fieldName].add(2);
            for (size_t index = 0; index < 256; index++) {
                longDoc[fieldName].add(index + 1);
            }
            for (size_t index = 0; index < 255; index++) {
                fullDoc[fieldName].add(index + 1);
                fullList[index] = index + 1;
            }
            for (size_t index = 0; index < 254; index++) {
                shortDoc[fieldName].add(index + 1);
                shortList[index] = index + 1;
            }
        });

        d.beforeEach([]() {
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = pwmLevels.deserialize(emptyDoc[fieldName]);
                    TEST_ASSERT_NULL(pwmLevels.get());
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = pwmLevels.deserialize(invalidDoc[fieldName]);
                    TEST_ASSERT_NULL(pwmLevels.get());
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("when the array is too long", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = pwmLevels.deserialize(longDoc[fieldName]);
                    TEST_ASSERT_NULL(pwmLevels.get());
                    TEST_ASSERT_EQUAL(tooLong, code);
                });
            });
            d.describe("when a level is not increasing", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = pwmLevels.deserialize(notIncreasingFieldDoc[fieldName]);
                    TEST_ASSERT_NULL(pwmLevels.get());
                    TEST_ASSERT_EQUAL(levelNotIncreasing, code);
                });
            });
            d.describe("when a level is zero", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = pwmLevels.deserialize(zeroFieldDoc[fieldName]);
                    TEST_ASSERT_NULL(pwmLevels.get());
                    TEST_ASSERT_EQUAL(levelZero, code);
                });
            });
            d.describe("when a level is missing", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = pwmLevels.deserialize(missingFieldDoc[fieldName]);
                    TEST_ASSERT_NULL(pwmLevels.get());
                    TEST_ASSERT_EQUAL(levelNotPresent, code);
                });
            });
            d.describe("when a level is invalid", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = pwmLevels.deserialize(invalidFieldDoc[fieldName]);
                    TEST_ASSERT_NULL(pwmLevels.get());
                    TEST_ASSERT_EQUAL(levelWrongType, code);
                });
            });
            d.describe("with a full array", [](Describe & d) {
                d.it("should have the correct values", []() {
                    auto code = pwmLevels.deserialize(fullDoc[fieldName]);
                    auto value = pwmLevels.get();
                    TEST_ASSERT_NOT_NULL(value);
                    auto list = value->uint8List;
                    for (size_t index = 0; index < fullList.size(); index++) {
                        TEST_ASSERT_EQUAL(fullList[index], list[index]);
                    }
                });
            });
            d.describe("with a short array", [](Describe & d) {
                d.it("should have the correct values", []() {
                    auto code = pwmLevels.deserialize(shortDoc[fieldName]);
                    auto value = pwmLevels.get();
                    TEST_ASSERT_NOT_NULL(value);
                    auto list = value->uint8List;
                    for (size_t index = 0; index < shortList.size(); index++) {
                        TEST_ASSERT_EQUAL(shortList[index], list[index]);
                    }
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    pwmLevels.set(&fullValue);
                    auto success = pwmLevels.serialize(tinyDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    pwmLevels.set(nullptr);
                    auto success = pwmLevels.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a full list of values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    pwmLevels.set(&fullValue);
                    auto success = pwmLevels.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    for (size_t index = 0; index < 255; index++) {
                        TEST_ASSERT_EQUAL(fullList[index], serializedDoc[fieldName][index]);
                    }
                    TEST_ASSERT_TRUE(serializedDoc[fieldName][255].isNull());
                });
            });
            d.describe("with a short list of values", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    pwmLevels.set(&shortValue);
                    auto success = pwmLevels.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    for (size_t index = 0; index < 254; index++) {
                        TEST_ASSERT_EQUAL(shortList[index], serializedDoc[fieldName][index]);
                    }
                    TEST_ASSERT_TRUE(serializedDoc[fieldName][254].isNull());
                });
            });
        });
    });

}