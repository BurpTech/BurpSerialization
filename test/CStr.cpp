#include <unity.h>
#include "../src/BurpSerialization/CStr.hpp"
#include "CStr.hpp"

namespace CStr {

    constexpr size_t length = 10;
    constexpr char fieldName[] = "field";
    constexpr int invalidValue = 100;
    constexpr char longValue[] = "01234567890";
    constexpr char validValue[] = "0123456789";

    enum : BurpStatus::Status::Code {
        ok,
        notPresent,
        wrongType,
        tooLong
    };

    BurpSerialization::CStr cstrRequired(length, true, {
        ok,
        notPresent,
        wrongType,
        tooLong
    });

    BurpSerialization::CStr cstrNotRequired(length, false, {
        ok,
        notPresent,
        wrongType,
        tooLong
    });

    StaticJsonDocument<128> emptyDoc;
    StaticJsonDocument<128> invalidDoc;
    StaticJsonDocument<128> longDoc;
    StaticJsonDocument<128> validDoc;
    StaticJsonDocument<128> serializedDoc;

    Module tests("CStr", [](Describe & d) {
        d.before([]() {
            invalidDoc[fieldName] = invalidValue;
            longDoc[fieldName] = longValue;
            validDoc[fieldName] = validValue;
        });

        d.beforeEach([]() {
            cstrNotRequired.set(validValue);
            cstrRequired.set(validValue);
        });

        d.describe("deserialize", [](Describe & d) {
            d.describe("when not present", [](Describe & d) {
                d.describe("when not required", [](Describe & d) {
                    d.it("should not fail but be null", []() {
                        const BurpStatus::Status::Code code = cstrNotRequired.deserialize(emptyDoc[fieldName]);
                        TEST_ASSERT_NULL(cstrNotRequired.get());
                        TEST_ASSERT_EQUAL(ok, code);
                    });
                });
                d.describe("when required", [](Describe & d) {
                    d.it("should fail and be null", []() {
                        const BurpStatus::Status::Code code = cstrRequired.deserialize(emptyDoc[fieldName]);
                        TEST_ASSERT_NULL(cstrRequired.get());
                        TEST_ASSERT_EQUAL(notPresent, code);
                    });
                });
            });
            d.describe("with an invalid value", [](Describe & d) {
                d.it("should fail and be null", []() {
                    const BurpStatus::Status::Code code = cstrRequired.deserialize(invalidDoc[fieldName]);
                    TEST_ASSERT_NULL(cstrRequired.get());
                    TEST_ASSERT_EQUAL(wrongType, code);
                });
            });
            d.describe("with a value that is too long", [](Describe & d) {
                d.it("should fail and be null", []() {
                    const BurpStatus::Status::Code code = cstrRequired.deserialize(longDoc[fieldName]);
                    TEST_ASSERT_NULL(cstrRequired.get());
                    TEST_ASSERT_EQUAL(tooLong, code);
                });
            });
        });
    });

}