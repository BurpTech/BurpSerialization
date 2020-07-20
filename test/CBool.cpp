#include <unity.h>
#include "../src/BurpSerialization/CBool.hpp"
#include "CBool.hpp"

namespace CBool {

    constexpr char fieldName[] = "field";
    constexpr int invalidValue = 100;
    constexpr BurpSerialization::Value trueValue = {.cbool=true};
    constexpr BurpSerialization::Value falseValue = {.cbool=false};

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
            cbool.set(&trueValue);
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = cbool.deserialize(emptyDoc[fieldName]);
                    TEST_ASSERT_NULL(cbool.get());
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    auto code = cbool.deserialize(invalidDoc[fieldName]);
                    TEST_ASSERT_NULL(cbool.get());
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with a true value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    cbool.set(nullptr);
                    auto code = cbool.deserialize(trueDoc[fieldName]);
                    auto value = cbool.get();
                    TEST_ASSERT_NOT_NULL(value);
                    TEST_ASSERT_TRUE(value->cbool);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
            d.describe("with a false value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    cbool.set(nullptr);
                    auto code = cbool.deserialize(falseDoc[fieldName]);
                    auto value = cbool.get();
                    TEST_ASSERT_NOT_NULL(value);
                    TEST_ASSERT_FALSE(value->cbool);
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    cbool.set(&trueValue);
                    auto success = cbool.serialize(tinyDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    serializedDoc[fieldName] = true;
                    cbool.set(nullptr);
                    auto success = cbool.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a true value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    cbool.set(&trueValue);
                    auto success = cbool.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName]);
                });
            });
            d.describe("with a false value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    cbool.set(&falseValue);
                    auto success = cbool.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_FALSE(serializedDoc[fieldName]);
                });
            });
        });
    });

}