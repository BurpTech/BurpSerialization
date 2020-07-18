#include <unity.h>
#include "../src/BurpSerialization/CBool.hpp"
#include "CBool.hpp"

namespace CBool {

    constexpr char fieldName[] = "field";
    constexpr int invalidValue = 100;

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
            cbool.set(true);
            serializedDoc.clear();
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    const BurpStatus::Status::Code code = cbool.deserialize(emptyDoc[fieldName]);
                    TEST_ASSERT_FALSE(cbool.isPresent());
                    TEST_ASSERT_FALSE(cbool.get());
                    TEST_ASSERT_EQUAL(notPresent, code);
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and not be present", []() {
                    const BurpStatus::Status::Code code = cbool.deserialize(invalidDoc[fieldName]);
                    TEST_ASSERT_FALSE(cbool.isPresent());
                    TEST_ASSERT_FALSE(cbool.get());
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with a true value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    cbool.set(false);
                    cbool.setNotPresent();
                    const BurpStatus::Status::Code code = cbool.deserialize(trueDoc[fieldName]);
                    TEST_ASSERT_TRUE(cbool.isPresent());
                    TEST_ASSERT_TRUE(cbool.get());
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
            d.describe("with a false value", [](Describe & d) {
                d.it("should not fail and have the correct value", []() {
                    cbool.set(true);
                    cbool.setNotPresent();
                    const BurpStatus::Status::Code code = cbool.deserialize(falseDoc[fieldName]);
                    TEST_ASSERT_TRUE(cbool.isPresent());
                    TEST_ASSERT_FALSE(cbool.get());
                    TEST_ASSERT_EQUAL(ok, code);
                });
            });
        });

        d.describe("set", [](Describe & d) {
            d.it("should set the value and set present", []() {
                cbool.set(false);
                cbool.setNotPresent();
                TEST_ASSERT_FALSE(cbool.isPresent());
                TEST_ASSERT_FALSE(cbool.get());
                cbool.set(true);
                TEST_ASSERT_TRUE(cbool.isPresent());
                TEST_ASSERT_TRUE(cbool.get());
                cbool.setNotPresent();
                TEST_ASSERT_FALSE(cbool.isPresent());
                cbool.set(false);
                TEST_ASSERT_TRUE(cbool.isPresent());
                TEST_ASSERT_FALSE(cbool.get());
            });
        });

        d.describe("serialize", [](Describe & d) {
            d.describe("with a value that is too big for the document", [](Describe & d) {
                d.it("should fail", []() {
                    cbool.set(true);
                    const bool success = cbool.serialize(tinyDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_FALSE(success);
                });
            });
            d.describe("without a value", [](Describe & d) {
                d.it("should set the value in the JSON document to NULL", []() {
                    serializedDoc[fieldName] = true;
                    cbool.setNotPresent();
                    const bool success = cbool.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName].isNull());
                });
            });
            d.describe("with a true value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    cbool.set(true);
                    const bool success = cbool.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_TRUE(serializedDoc[fieldName]);
                });
            });
            d.describe("with a false value", [](Describe & d) {
                d.it("should set the value in the JSON document", []() {
                    cbool.set(false);
                    const bool success = cbool.serialize(serializedDoc[fieldName].to<JsonVariant>());
                    TEST_ASSERT_TRUE(success);
                    TEST_ASSERT_FALSE(serializedDoc[fieldName]);
                });
            });
        });
    });

}