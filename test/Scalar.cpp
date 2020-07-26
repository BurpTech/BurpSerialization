#include <unity.h>
#include <functional>
#include "../src/BurpSerialization/Scalar.hpp"
#include "Scalar.hpp"

namespace Scalar {

    constexpr char fieldName[] = "field";

    enum : BurpStatus::Status::Code {
        ok,
        notPresent,
        wrongType
    };

    template <class Type, size_t valueCount, size_t docSize>
    class Tests {

    public:

        struct Value {
            const char * description;
            const Type value;
        };
        using Values = std::array<Value, valueCount>;
        using f_get = std::function<Type(const BurpSerialization::Value & value)>;

        Tests(const char * description, const Values values, f_get get) :
            _scalar({
                ok,
                notPresent,
                wrongType
            }),
            _description(description),
            _get(get)
        {
            for (size_t index = 0; index < valueCount; index++) {
                const Value value = values[index];
                _values[index].description = value.description;
                _values[index].typedValue = value.value;
                _values[index].value = value.value;
                _values[index].doc[fieldName] = value.value;
            }
            _invalidDoc[fieldName] = "hello";
        }

        void test(Describe & d) {
            d.describe(_description, [&](Describe & d){
                d.beforeEach([&]() {
                    _serializedDoc.clear();
                });

                d.describe("deserialize", [&](Describe & d) {
                    d.describe("when not present", [&](Describe & d) {
                        d.it("should fail and not be present", [&]() {
                            BurpSerialization::Value value;
                            auto code = _scalar.deserialize(value, _emptyDoc[fieldName]);
                            TEST_ASSERT_TRUE(value.isNull);
                            TEST_ASSERT_EQUAL(notPresent, code);
                        });
                    });
                    d.describe("with an invalid value", [&](Describe & d) {
                        d.it("should fail and not be present", [&]() {
                            BurpSerialization::Value value;
                            auto code = _scalar.deserialize(value, _invalidDoc[fieldName]);
                            TEST_ASSERT_TRUE(value.isNull);
                            TEST_ASSERT_EQUAL(wrongType, code);
                        });
                    });

                    for (auto & internalValue : _values) {
                        d.describe(internalValue.description, [&](Describe & d) {
                            d.it("should not fail and have the correct value", [&]() {
                                BurpSerialization::Value value;
                                auto code = _scalar.deserialize(value, internalValue.doc[fieldName]);
                                TEST_ASSERT_FALSE(value.isNull);
                                TEST_ASSERT_EQUAL(internalValue.typedValue, _get(value));
                                TEST_ASSERT_EQUAL(ok, code);
                            });
                        });
                    }

                });

                d.describe("serialize", [&](Describe & d) {
                    d.describe("when the document is too small", [&](Describe & d) {
                        for (auto & internalValue : _values) {
                            d.describe(internalValue.description, [&](Describe & d) {
                                d.it("should fail", [&]() {
                                    auto success = _scalar.serialize(_tinyDoc[fieldName].template to<JsonVariant>(), internalValue.value);
                                    TEST_ASSERT_FALSE(success);
                                });
                            });
                        }
                    });
                    d.describe("when the document is big enough", [&](Describe & d) {
                        for (auto & internalValue : _values) {
                            d.describe(internalValue.description, [&](Describe & d) {
                                d.it("should set the value in the JSON document", [&]() {
                                    auto success = _scalar.serialize(_serializedDoc[fieldName].template to<JsonVariant>(), internalValue.value);
                                    TEST_ASSERT_TRUE(success);
                                    TEST_ASSERT_EQUAL(internalValue.typedValue, _serializedDoc[fieldName]);
                                });
                            });
                        }
                    });
                    d.describe("without a value", [&](Describe & d) {
                        d.it("should set the value in the JSON document to NULL", [&]() {
                            _serializedDoc[fieldName] = "hello";
                            auto success = _scalar.serialize(_serializedDoc[fieldName].template to<JsonVariant>(), _nullValue);
                            TEST_ASSERT_TRUE(success);
                            TEST_ASSERT_TRUE(_serializedDoc[fieldName].isNull());
                        });
                    });
                });
            });
        }

    private:

        struct InternalValue {
            const char * description;
            Type typedValue;
            BurpSerialization::Value value;
            StaticJsonDocument<docSize> doc;
        };
        using InternalValues = std::array<InternalValue, valueCount>;

        using Scalar = BurpSerialization::Scalar<Type>;
        Scalar _scalar;

        const char * _description;
        f_get _get;
        InternalValues _values;
        StaticJsonDocument<1> _emptyDoc;
        StaticJsonDocument<docSize> _invalidDoc;
        StaticJsonDocument<docSize> _serializedDoc;
        StaticJsonDocument<1> _tinyDoc;
        const BurpSerialization::Value _nullValue = {true};

    };

    using CBoolTests = Tests<bool, 2, 128>;
    CBoolTests cboolTests(
        "CBool",
        CBoolTests::Values({
            CBoolTests::Value({"with true", true}),
            CBoolTests::Value({"with false", false})
        }),
        [](BurpSerialization::Value value) {return value.cbool;}
    );

    using UInt8Tests = Tests<uint8_t, 2, 128>;
    UInt8Tests uint8Tests(
        "UInt8",
        UInt8Tests::Values({
            UInt8Tests::Value({"with zero", 0}),
            UInt8Tests::Value({"with max", UINT8_MAX})
        }),
        [](BurpSerialization::Value value) {return value.uint8;}
    );

    using UInt16Tests = Tests<uint16_t, 2, 128>;
    UInt16Tests uint16Tests(
        "UInt16",
        UInt16Tests::Values({
            UInt16Tests::Value({"with zero", 0}),
            UInt16Tests::Value({"with max", UINT16_MAX})
        }),
        [](BurpSerialization::Value value) {return value.uint16;}
    );

    using UInt32Tests = Tests<uint32_t, 2, 128>;
    UInt32Tests uint32Tests(
        "UInt32",
        UInt32Tests::Values({
            UInt32Tests::Value({"with zero", 0}),
            UInt32Tests::Value({"with max", UINT32_MAX})
        }),
        [](BurpSerialization::Value value) {return value.uint32;}
    );

    using Int8Tests = Tests<int8_t, 2, 128>;
    Int8Tests int8Tests(
        "Int8",
        Int8Tests::Values({
            Int8Tests::Value({"with min", INT8_MIN}),
            Int8Tests::Value({"with max", INT8_MAX})
        }),
        [](BurpSerialization::Value value) {return value.int8;}
    );

    using Int16Tests = Tests<int16_t, 2, 128>;
    Int16Tests int16Tests(
        "Int16",
        Int16Tests::Values({
            Int16Tests::Value({"with min", INT16_MIN}),
            Int16Tests::Value({"with max", INT16_MAX})
        }),
        [](BurpSerialization::Value value) {return value.int16;}
    );

    using Int32Tests = Tests<int32_t, 2, 128>;
    Int32Tests int32Tests(
        "Int32",
        Int32Tests::Values({
            Int32Tests::Value({"with min", INT32_MIN}),
            Int32Tests::Value({"with max", INT32_MAX})
        }),
        [](BurpSerialization::Value value) {return value.int32;}
    );

    Module tests("Scalar", [](Describe & d) {
        cboolTests.test(d);
        uint8Tests.test(d);
        uint16Tests.test(d);
        uint32Tests.test(d);
        int8Tests.test(d);
        int16Tests.test(d);
        int32Tests.test(d);
    });

}