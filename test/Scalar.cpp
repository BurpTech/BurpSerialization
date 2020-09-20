#include <unity.h>
#include <functional>
#include "../src/BurpSerialization/Serialization.hpp"
#include "../src/BurpSerialization/Scalar.hpp"
#include "Scalar.hpp"

namespace Scalar {

    template <class Type>
    class Serialization : public BurpSerialization::Serialization {

        public:

            enum : BurpStatus::Status::Code {
                ok,
                notPresent,
                wrongType
            };

            typename BurpSerialization::Scalar<Type>::Value scalar;

            Serialization() :
                BurpSerialization::Serialization(_scalar),
                _scalar({
                    ok,
                    notPresent,
                    wrongType
                }, scalar)
            {}

        private:

            const BurpSerialization::Scalar<Type> _scalar;

    };

    constexpr size_t docSize = 128;
    constexpr char fieldName[] = "field";

    template <class Type, size_t valueCount, size_t docSize>
    class Tests {

    public:

        struct Scenario {
            const char * description;
            const Type value;
        };
        using Scenarios = std::array<Scenario, valueCount>;

        Tests(const char * description, const Scenarios values) :
            _description(description),
            _scenarios(values)
        {}

        void test(Describe & d) {
            d.describe(_description, [&](Describe & d){
                d.describe("deserialize", [&](Describe & d) {
                    d.describe("when not present", [&](Describe & d) {
                        d.it("should fail and not be present", [&]() {
                            Serialization<Type> serialization;
                            StaticJsonDocument<1> doc;
                            auto code = serialization.deserialize(doc[fieldName]);
                            TEST_ASSERT_TRUE(serialization.scalar.isNull);
                            TEST_ASSERT_EQUAL(Serialization<Type>::notPresent, code);
                        });
                    });
                    d.describe("with an invalid value", [&](Describe & d) {
                        d.it("should fail and not be present", [&]() {
                            Serialization<Type> serialization;
                            StaticJsonDocument<docSize> doc;
                            doc[fieldName] = "hello";
                            auto code = serialization.deserialize(doc[fieldName]);
                            TEST_ASSERT_TRUE(serialization.scalar.isNull);
                            TEST_ASSERT_EQUAL(Serialization<Type>::wrongType, code);
                        });
                    });

                    for (auto & scenario : _scenarios) {
                        d.describe(scenario.description, [&](Describe & d) {
                            d.it("should not fail and have the correct value", [&]() {
                                Serialization<Type> serialization;
                                StaticJsonDocument<docSize> doc;
                                doc[fieldName] = scenario.value;
                                auto code = serialization.deserialize(doc[fieldName]);
                                TEST_ASSERT_FALSE(serialization.scalar.isNull);
                                TEST_ASSERT_EQUAL(scenario.value, serialization.scalar.value);
                                TEST_ASSERT_EQUAL(Serialization<Type>::ok, code);
                            });
                        });
                    }

                });

                d.describe("serialize", [&](Describe & d) {
                    d.describe("when the document is too small", [&](Describe & d) {
                        for (auto & scenario : _scenarios) {
                            d.describe(scenario.description, [&](Describe & d) {
                                d.it("should fail", [&]() {
                                    Serialization<Type> serialization;
                                    StaticJsonDocument<1> doc;
                                    serialization.scalar.isNull = false;
                                    serialization.scalar.value = scenario.value;
                                    auto success = serialization.serialize(doc[fieldName].template to<JsonVariant>());
                                    TEST_ASSERT_FALSE(success);
                                });
                            });
                        }
                    });
                    d.describe("when the document is big enough", [&](Describe & d) {
                        for (auto & scenario : _scenarios) {
                            d.describe(scenario.description, [&](Describe & d) {
                                d.it("should set the value in the JSON document", [&]() {
                                    Serialization<Type> serialization;
                                    StaticJsonDocument<docSize> doc;
                                    serialization.scalar.isNull = false;
                                    serialization.scalar.value = scenario.value;
                                    auto success = serialization.serialize(doc[fieldName].template to<JsonVariant>());
                                    TEST_ASSERT_TRUE(success);
                                    TEST_ASSERT_EQUAL(scenario.value, doc[fieldName].template as<Type>());
                                });
                            });
                        }
                    });
                    d.describe("without a value", [&](Describe & d) {
                        d.it("should set the value in the JSON document to NULL", [&]() {
                            Serialization<Type> serialization;
                            StaticJsonDocument<docSize> doc;
                            doc[fieldName] = "hello";
                            serialization.scalar.isNull = true;
                            auto success = serialization.serialize(doc[fieldName].template to<JsonVariant>());
                            TEST_ASSERT_TRUE(success);
                            TEST_ASSERT_TRUE(doc[fieldName].isNull());
                        });
                    });
                });
            });
        }

    private:

        const char * _description;
        const Scenarios _scenarios;

    };

    using CBoolTests = Tests<bool, 2, docSize>;
    CBoolTests cboolTests(
        "CBool",
        CBoolTests::Scenarios({
            CBoolTests::Scenario({"with true", true}),
            CBoolTests::Scenario({"with false", false})
        })
    );

    using UInt8Tests = Tests<uint8_t, 2, docSize>;
    UInt8Tests uint8Tests(
        "UInt8",
        UInt8Tests::Scenarios({
            UInt8Tests::Scenario({"with zero", 0}),
            UInt8Tests::Scenario({"with max", UINT8_MAX})
        })
    );

    using UInt16Tests = Tests<uint16_t, 2, docSize>;
    UInt16Tests uint16Tests(
        "UInt16",
        UInt16Tests::Scenarios({
            UInt16Tests::Scenario({"with zero", 0}),
            UInt16Tests::Scenario({"with max", UINT16_MAX})
        })
    );

    using UInt32Tests = Tests<uint32_t, 2, docSize>;
    UInt32Tests uint32Tests(
        "UInt32",
        UInt32Tests::Scenarios({
            UInt32Tests::Scenario({"with zero", 0}),
            UInt32Tests::Scenario({"with max", UINT32_MAX})
        })
    );

    using Int8Tests = Tests<int8_t, 2, docSize>;
    Int8Tests int8Tests(
        "Int8",
        Int8Tests::Scenarios({
            Int8Tests::Scenario({"with min", INT8_MIN}),
            Int8Tests::Scenario({"with max", INT8_MAX})
        })
    );

    using Int16Tests = Tests<int16_t, 2, docSize>;
    Int16Tests int16Tests(
        "Int16",
        Int16Tests::Scenarios({
            Int16Tests::Scenario({"with min", INT16_MIN}),
            Int16Tests::Scenario({"with max", INT16_MAX})
        })
    );

    using Int32Tests = Tests<int32_t, 2, docSize>;
    Int32Tests int32Tests(
        "Int32",
        Int32Tests::Scenarios({
            Int32Tests::Scenario({"with min", INT32_MIN}),
            Int32Tests::Scenario({"with max", INT32_MAX})
        })
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