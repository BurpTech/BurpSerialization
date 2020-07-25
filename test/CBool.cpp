#include <unity.h>
#include "../src/BurpSerialization/CBool.hpp"
#include "CBool.hpp"

namespace CBool {

    constexpr char fieldName[] = "field";
    constexpr int invalidValue = 100;
    constexpr BurpSerialization::Value nullValue = {true};
    constexpr BurpSerialization::Value trueValue = {false, {.cbool=true}};
    constexpr BurpSerialization::Value falseValue = {false, {.cbool=false}};

    enum : BurpStatus::Status::Code {
        ok,
        notPresent,
        wrongType
    };

    BurpSerialization::CBool cbool({
        ok,
        notPresent,
        wrongType
    });

    StaticJsonDocument<128> emptyDoc;
    StaticJsonDocument<128> invalidDoc;
    StaticJsonDocument<128> trueDoc;
    StaticJsonDocument<128> falseDoc;
    StaticJsonDocument<128> serializedDoc;
    StaticJsonDocument<1> tinyDoc;

    Module tests("CBool", [](Describe & d) {
        d.before([]() {
            invalidDoc[fieldName] = invalidValue;
            trueDoc[fieldName] = true;
            falseDoc[fieldName] = false;
        });

        d.beforeEach([]() {
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    BurpSerialization::Value value;
                    auto code = cbool.deserialize(value, emptyDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    BurpSerialization::Value value;
                    auto code = cbool.deserialize(value, invalidDoc[fieldName]);
                    TEST_ASSERT_TRUE(value.isNull);
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with a true value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    BurpSerialization::Value value;
                    auto code = cbool.deserialize(value, trueDoc[fieldName]);
                    TEST_ASSERT_FALSE(value.isNull);
                    TEST_ASSERT_TRUE(value.cbool);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
            d.describe("with a false value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    BurpSerialization::Value value;
                    auto code = cbool.deserialize(value, falseDoc[fieldName]);
                    TEST_ASSERT_FALSE(value.isNull);
                    TEST_ASSERT_FALSE(value.cbool);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    BurpSerialization::Value value = {false, {.cbool=true}};
                    auto success = cbool.serialize(tinyDoc[fieldName].to<JsonVariant>(), value);
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    serializedDoc[fieldName] = true;
                    auto success = cbool.serialize(serializedDoc[fieldName].to<JsonVariant>(), nullValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a true value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    auto success = cbool.serialize(serializedDoc[fieldName].to<JsonVariant>(), trueValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName]);
                });
            });
            d.describe("with a false value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    auto success = cbool.serialize(serializedDoc[fieldName].to<JsonVariant>(), falseValue);
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_FALSE(serializedDoc[fieldName]);
                });
            });
        });
    });

}