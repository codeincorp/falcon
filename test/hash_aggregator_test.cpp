#include <gtest/gtest.h>
#include <algorithm>
#include <any>
#include <memory>
#include <numeric>
#include <unordered_map>
#include <utility>

#include "iterator.h"
#include "hash_aggregator.h"
#include "projector.h"
#include "mock_scanner.h"

using namespace std;
using namespace codein;

namespace std {

using KeyType = pair<int, string>;

template <>
struct hash<KeyType> {
    size_t operator()(const KeyType& key) const
    {
        return hash<int>()(key.first) ^ hash<string>()(key.second);
    }
};

}

struct HashAggregatorTests : public ::testing::Test {
    static Metadata metadata;
    static vector<string> lines;
    static unordered_multimap<KeyType, double> expectedDataMap;

    static void SetUpTestSuite()
    {
        metadata = {
            { "a", tiInt },
            { "b", tiString },
            { "c", tiDouble },
        };

        lines = {
            "1,A,3.14",
            "1,A,8.82",
            "1,A,100.23",
            "1,A,-19.3",            // group #1
            "1,B,2.4",
            "1,B,-1.8",             // group #2
            "2,C,4.0",
            "2,C,-3.0",
            "2,C,9.1",              // group #3
            "2,A,38.9",             // group #4
            "3,C,-3.9",
            "3,C,3.7",              // group #5
            "3,E,10.9",             // group #6
            "3,D,1.2",              // group #7
        };

        auto mockScanner = makeIterator<MockScanner>(metadata, lines);
        mockScanner->open();
        while (mockScanner->hasMore()) {
            auto optData = mockScanner->processNext();
            if (!optData) {
                break;
            }

            auto a = any_cast<int>((*optData)[0]);
            auto b = any_cast<string>((*optData)[1]);
            auto c = any_cast<double>((*optData)[2]);
            expectedDataMap.emplace(make_pair(a, b), c);
        }
    }
};

Metadata HashAggregatorTests::metadata;
vector<string> HashAggregatorTests::lines;
unordered_multimap<KeyType, double> HashAggregatorTests::expectedDataMap;

TEST_F(HashAggregatorTests, BasicTest)
{
    Metadata groupValMetadata{
        {"count", tiUint}
    };
    auto groupKeyCols = vector<string>{"a", "b"};

    // count(*) aggregation
    vector<AggregationExpression> aggExprs{
        AggregationExpression{
            // The initial value when a group is created
            .initExpr = {
                .opCode = OpCode::Const,
                .leafOrChildren = std::any(1u),
            },
            // Add 1 whenever a new member is added
            .contExpr = {
                .opCode = OpCode::Add,
                .leafOrChildren = vector<Expression>{
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("count"s)},
                    {.opCode = OpCode::Const, .leafOrChildren = std::any(1u)},
                }
            }
        },
    };

    auto mockScanner = makeIterator<MockScanner>(metadata, lines);
    auto hashAggregator = makeIterator<HashAggregator>(
        move(mockScanner), groupKeyCols, groupValMetadata, aggExprs);

    Metadata expectedOutputMetadata{
        {"a", tiInt},
        {"b", tiString},
        {"count", tiUint},
    };
    EXPECT_TRUE(hashAggregator->getMetadata() == expectedOutputMetadata);

    hashAggregator->open();
    size_t n = 0;
    const size_t expectedNumData = 7;
    while (hashAggregator->hasMore()) {
        auto optData = hashAggregator->processNext();
        if (!optData) {
            break;
        }

        auto a = any_cast<int>((*optData)[0]);
        auto b = any_cast<string>((*optData)[1]);
        auto count = any_cast<unsigned>((*optData)[2]);

        EXPECT_EQ(count, expectedDataMap.count(make_pair(a, b)));

        ++n;
    }

    EXPECT_EQ(n, expectedNumData);
}

TEST_F(HashAggregatorTests, MaxTest)
{
    Metadata groupValMetadata{
        {"maxc", tiUint}
    };
    auto groupKeyCols = vector<string>{"a", "b"};

    // max(c) aggregation
    vector<AggregationExpression> aggExprs{
        AggregationExpression{
            // The initial value is set to the first value of "c" when a group is created
            .initExpr = {
                .opCode = OpCode::Ref,
                .leafOrChildren = std::any("c"s),
            },
            // maxc > c ? maxc : c
            .contExpr = {
                .opCode = OpCode::Cond,
                .leafOrChildren = vector<Expression>{
                    {
                        .opCode = OpCode::Gt,
                        .leafOrChildren = vector<Expression>{
                            {.opCode = OpCode::Ref, .leafOrChildren = std::any("maxc"s)},
                            {.opCode = OpCode::Ref, .leafOrChildren = std::any("c"s)},
                        }
                    },
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("maxc"s)},
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("c"s)},
                }
            }
        },
    };

    auto mockScanner = makeIterator<MockScanner>(metadata, lines);
    auto hashAggregator = makeIterator<HashAggregator>(
        move(mockScanner), groupKeyCols, groupValMetadata, aggExprs);

    Metadata expectedOutputMetadata{
        {"a", tiInt},
        {"b", tiString},
        {"maxc", tiUint},
    };
    EXPECT_TRUE(hashAggregator->getMetadata() == expectedOutputMetadata);

    hashAggregator->open();
    size_t n = 0;
    const size_t expectedNumData = 7;
    while (hashAggregator->hasMore()) {
        auto optData = hashAggregator->processNext();
        if (!optData) {
            break;
        }

        auto a = any_cast<int>((*optData)[0]);
        auto b = any_cast<string>((*optData)[1]);
        auto maxVal = any_cast<double>((*optData)[2]);

        auto range = expectedDataMap.equal_range(make_pair(a, b));
        auto expectedMaxElem = max_element(range.first, range.second, [](const auto& lhs, const auto& rhs) {
            return lhs.second < rhs.second;
        });
        EXPECT_EQ(maxVal, expectedMaxElem->second);

        ++n;
    }

    EXPECT_EQ(n, expectedNumData);
}

TEST_F(HashAggregatorTests, MinTest)
{
    Metadata groupValMetadata{
        {"minc", tiUint}
    };
    auto groupKeyCols = vector<string>{"a", "b"};

    // min(c) aggregation
    vector<AggregationExpression> aggExprs{
        AggregationExpression{
            // The initial value is set to the first value of "c" when a group is created
            .initExpr = {
                .opCode = OpCode::Ref,
                .leafOrChildren = std::any("c"s),
            },
            // minc < c ? minc : c
            .contExpr = {
                .opCode = OpCode::Cond,
                .leafOrChildren = vector<Expression>{
                    {
                        .opCode = OpCode::Lt,
                        .leafOrChildren = vector<Expression>{
                            {.opCode = OpCode::Ref, .leafOrChildren = std::any("minc"s)},
                            {.opCode = OpCode::Ref, .leafOrChildren = std::any("c"s)},
                        }
                    },
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("minc"s)},
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("c"s)},
                }
            }
        },
    };

    auto mockScanner = makeIterator<MockScanner>(metadata, lines);
    auto hashAggregator = makeIterator<HashAggregator>(
        move(mockScanner), groupKeyCols, groupValMetadata, aggExprs);

    Metadata expectedOutputMetadata{
        {"a", tiInt},
        {"b", tiString},
        {"minc", tiUint},
    };
    EXPECT_TRUE(hashAggregator->getMetadata() == expectedOutputMetadata);

    hashAggregator->open();
    size_t n = 0;
    const size_t expectedNumData = 7;
    while (hashAggregator->hasMore()) {
        auto optData = hashAggregator->processNext();
        if (!optData) {
            break;
        }

        auto a = any_cast<int>((*optData)[0]);
        auto b = any_cast<string>((*optData)[1]);
        auto minVal = any_cast<double>((*optData)[2]);

        auto range = expectedDataMap.equal_range(make_pair(a, b));
        auto expectedMinElem = min_element(range.first, range.second, [](const auto& lhs, const auto& rhs) {
            return lhs.second < rhs.second;
        });
        EXPECT_EQ(minVal, expectedMinElem->second);

        ++n;
    }

    EXPECT_EQ(n, expectedNumData);
}

TEST_F(HashAggregatorTests, SumTest)
{
    Metadata groupValMetadata{
        {"sumc", tiUint}
    };
    auto groupKeyCols = vector<string>{"a", "b"};

    // sum(c) aggregation
    vector<AggregationExpression> aggExprs{
        AggregationExpression{
            // The initial value is set to the first value of "c" when a group is created
            .initExpr = {
                .opCode = OpCode::Ref,
                .leafOrChildren = std::any("c"s),
            },
            // sumc + c
            .contExpr = {
                .opCode = OpCode::Add,
                .leafOrChildren = vector<Expression>{
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("sumc"s)},
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("c"s)},
                }
            }
        },
    };

    auto mockScanner = makeIterator<MockScanner>(metadata, lines);
    auto hashAggregator = makeIterator<HashAggregator>(
        move(mockScanner), groupKeyCols, groupValMetadata, aggExprs);

    Metadata expectedOutputMetadata{
        {"a", tiInt},
        {"b", tiString},
        {"sumc", tiUint},
    };
    EXPECT_TRUE(hashAggregator->getMetadata() == expectedOutputMetadata);

    hashAggregator->open();
    size_t n = 0;
    const size_t expectedNumData = 7;
    while (hashAggregator->hasMore()) {
        auto optData = hashAggregator->processNext();
        if (!optData) {
            break;
        }

        auto a = any_cast<int>((*optData)[0]);
        auto b = any_cast<string>((*optData)[1]);
        auto sumVal = any_cast<double>((*optData)[2]);

        auto range = expectedDataMap.equal_range(make_pair(a, b));
        auto expectedSum = accumulate(range.first, range.second, 0.0, [](auto lhs, const auto& rhs) {
            return lhs + rhs.second;
        });
        EXPECT_EQ(sumVal, expectedSum);

        ++n;
    }

    EXPECT_EQ(n, expectedNumData);
}

TEST_F(HashAggregatorTests, ProjOverHashAggTest)
{
    Metadata groupValMetadata{
        {"count", tiUint},
        {"sumc", tiDouble}
    };
    auto groupKeyCols = vector<string>{"a", "b"};

    vector<AggregationExpression> aggExprs{
        // count(*) aggregation
        AggregationExpression{
            // The initial value when a group is created
            .initExpr = {
                .opCode = OpCode::Const,
                .leafOrChildren = std::any(1u),
            },
            // Add 1 whenever a new member is added
            .contExpr = {
                .opCode = OpCode::Add,
                .leafOrChildren = vector<Expression>{
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("count"s)},
                    {.opCode = OpCode::Const, .leafOrChildren = std::any(1u)},
                }
            }
        },
        // sum(c) aggregation
        AggregationExpression{
            // The initial value is set to the first value of "c" when a group is created
            .initExpr = {
                .opCode = OpCode::Ref,
                .leafOrChildren = std::any("c"s),
            },
            // sumc + c
            .contExpr = {
                .opCode = OpCode::Add,
                .leafOrChildren = vector<Expression>{
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("sumc"s)},
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("c"s)},
                }
            }
        },
    };

    auto mockScanner = makeIterator<MockScanner>(metadata, lines);
    auto hashAggregator = makeIterator<HashAggregator>(
        move(mockScanner), groupKeyCols, groupValMetadata, aggExprs);

    Metadata expectedOutputMetadata{
        {"a", tiInt},
        {"b", tiString},
        {"count", tiUint},
        {"sumc", tiDouble},
    };
    EXPECT_TRUE(hashAggregator->getMetadata() == expectedOutputMetadata);

    vector<Expression> projections{
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("b"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("count"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("sumc"s)},
        {
            .opCode = OpCode::Div,
            .leafOrChildren = vector<Expression>{
                {.opCode = OpCode::Ref, .leafOrChildren = std::any("sumc"s)},
                // conv(count, "double")
                // sumc is double and so count must be converted to double before division.
                {
                    .opCode = OpCode::Conv,
                    .leafOrChildren = vector<Expression>{
                        {.opCode = OpCode::Ref, .leafOrChildren = std::any("count"s)},
                        {.opCode = OpCode::Const, .leafOrChildren = std::any("double"s)},
                    }
                },
            },
        },
    };
    Metadata projMetadata{
        {"a", tiInt},
        {"b", tiString},
        {"count", tiUint},
        {"sumc", tiDouble},
        {"avg", tiDouble}
    };
    auto projector = makeIterator<Projector>(move(hashAggregator), projections, projMetadata);

    projector->open();
    size_t n = 0;
    const size_t expectedNumData = 7;
    while (projector->hasMore()) {
        auto optData = projector->processNext();
        if (!optData) {
            break;
        }

        auto a = any_cast<int>((*optData)[0]);
        auto b = any_cast<string>((*optData)[1]);
        auto count = any_cast<unsigned>((*optData)[2]);
        auto sumVal = any_cast<double>((*optData)[3]);
        auto avg = any_cast<double>((*optData)[4]);

        auto groupKey = make_pair(a, b);
        auto expectedCount = expectedDataMap.count(groupKey);
        auto range = expectedDataMap.equal_range(groupKey);
        auto expectedSum = accumulate(range.first, range.second, 0.0, [](auto lhs, const auto& rhs) {
            return lhs + rhs.second;
        });

        EXPECT_EQ(count, expectedCount);
        EXPECT_EQ(sumVal, expectedSum);
        EXPECT_DOUBLE_EQ(avg, expectedSum / static_cast<double>(expectedCount));

        ++n;
    }

    EXPECT_EQ(n, expectedNumData);
}

TEST_F(HashAggregatorTests, AverageTest)
{
    Metadata groupValMetadata{
        {"count", tiUint},
        {"sumc", tiDouble}
    };
    Metadata outputMetadata{
        {"a", tiInt},
        {"b", tiString},
        {"avg", tiDouble}
    };
    auto groupKeyCols = vector<string>{"a", "b"};

    vector<AggregationExpression> aggExprs{
        // count(*) aggregation
        AggregationExpression{
            // The initial value when a group is created
            .initExpr = {
                .opCode = OpCode::Const,
                .leafOrChildren = std::any(1u),
            },
            // Add 1 whenever a new member is added
            .contExpr = {
                .opCode = OpCode::Add,
                .leafOrChildren = vector<Expression>{
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("count"s)},
                    {.opCode = OpCode::Const, .leafOrChildren = std::any(1u)},
                }
            }
        },
        // sum(c) aggregation
        AggregationExpression{
            // The initial value is set to the first value of "c" when a group is created
            .initExpr = {
                .opCode = OpCode::Ref,
                .leafOrChildren = std::any("c"s),
            },
            // sumc + c
            .contExpr = {
                .opCode = OpCode::Add,
                .leafOrChildren = vector<Expression>{
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("sumc"s)},
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("c"s)},
                }
            }
        },
    };

    vector<Expression> projections{
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("b"s)},
        {
            .opCode = OpCode::Div,
            .leafOrChildren = vector<Expression>{
                {.opCode = OpCode::Ref, .leafOrChildren = std::any("sumc"s)},
                // conv(count, "double")
                // sumc is double and so count must be converted to double before division.
                {
                    .opCode = OpCode::Conv,
                    .leafOrChildren = vector<Expression>{
                        {.opCode = OpCode::Ref, .leafOrChildren = std::any("count"s)},
                        {.opCode = OpCode::Const, .leafOrChildren = std::any("double"s)},
                    }
                },
            },
        },
    };

    auto mockScanner = makeIterator<MockScanner>(metadata, lines);
    auto hashAggregator = makeIterator<HashAggregator>(
        move(mockScanner), groupKeyCols, groupValMetadata, outputMetadata, aggExprs, projections);

    EXPECT_TRUE(hashAggregator->getMetadata() == outputMetadata);

    hashAggregator->open();
    size_t n = 0;
    const size_t expectedNumData = 7;
    while (hashAggregator->hasMore()) {
        auto optData = hashAggregator->processNext();
        if (!optData) {
            break;
        }

        auto a = any_cast<int>((*optData)[0]);
        auto b = any_cast<string>((*optData)[1]);
        auto avg = any_cast<double>((*optData)[2]);

        auto groupKey = make_pair(a, b);
        auto expectedCount = expectedDataMap.count(groupKey);
        auto range = expectedDataMap.equal_range(groupKey);
        auto expectedSum = accumulate(range.first, range.second, 0.0, [](auto lhs, const auto& rhs) {
            return lhs + rhs.second;
        });

        EXPECT_DOUBLE_EQ(avg, expectedSum / static_cast<double>(expectedCount));

        ++n;
    }

    EXPECT_EQ(n, expectedNumData);
}
