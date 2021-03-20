#include <gtest/gtest.h>
#include <any>
#include <memory>

#include "iterator.h"
#include "hash_aggregator.h"
#include "mock_scanner.h"

using namespace std;
using namespace codein;

struct HashAggregatorTests : public ::testing::Test {
    Metadata metadata{
        { "a", tiInt },
        { "b", tiString },
        { "c", tiDouble },
    };

    vector<string> lines{
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
};

TEST_F(HashAggregatorTests, BasicTest)
{
    Metadata outputMetadata{
        {"a", tiInt},
        {"b", tiString},
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
    auto hashAggregator = makeIterator<HashAggregator>(move(mockScanner), outputMetadata, groupKeyCols, aggExprs);

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

        if (a == 1 && b == "A"s) {
            EXPECT_EQ(count, 4);
        }
        else if (a == 1 && b == "B"s) {
            EXPECT_EQ(count, 2);
        }
        else if (a == 2 && b == "C"s) {
            EXPECT_EQ(count, 3);
        }
        else if (a == 2 && b == "A"s) {
            EXPECT_EQ(count, 1);
        }
        else if (a == 3 && b == "C"s) {
            EXPECT_EQ(count, 2);
        }
        else if (a == 3 && b == "E"s) {
            EXPECT_EQ(count, 1);
        }
        else if (a == 3 && b == "D"s) {
            EXPECT_EQ(count, 1);
        }
        else {
            EXPECT_TRUE(false) << "must not reach here";
        }

        ++n;
    }

    EXPECT_EQ(n, expectedNumData);
}

TEST_F(HashAggregatorTests, MaxTest)
{
    Metadata outputMetadata{
        {"a", tiInt},
        {"b", tiString},
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
    auto hashAggregator = makeIterator<HashAggregator>(move(mockScanner), outputMetadata, groupKeyCols, aggExprs);

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

        if (a == 1 && b == "A"s) {
            EXPECT_EQ(maxVal, 100.23);
        }
        else if (a == 1 && b == "B"s) {
            EXPECT_EQ(maxVal, 2.4);
        }
        else if (a == 2 && b == "C"s) {
            EXPECT_EQ(maxVal, 9.1);
        }
        else if (a == 2 && b == "A"s) {
            EXPECT_EQ(maxVal, 38.9);
        }
        else if (a == 3 && b == "C"s) {
            EXPECT_EQ(maxVal, 3.7);
        }
        else if (a == 3 && b == "E"s) {
            EXPECT_EQ(maxVal, 10.9);
        }
        else if (a == 3 && b == "D"s) {
            EXPECT_EQ(maxVal, 1.2);
        }
        else {
            EXPECT_TRUE(false) << "must not reach here";
        }

        ++n;
    }

    EXPECT_EQ(n, expectedNumData);
}

TEST_F(HashAggregatorTests, MinTest)
{
    Metadata outputMetadata{
        {"a", tiInt},
        {"b", tiString},
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
    auto hashAggregator = makeIterator<HashAggregator>(move(mockScanner), outputMetadata, groupKeyCols, aggExprs);

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

        if (a == 1 && b == "A"s) {
            EXPECT_EQ(minVal, -19.3);
        }
        else if (a == 1 && b == "B"s) {
            EXPECT_EQ(minVal, -1.8);
        }
        else if (a == 2 && b == "C"s) {
            EXPECT_EQ(minVal, -3.0);
        }
        else if (a == 2 && b == "A"s) {
            EXPECT_EQ(minVal, 38.9);
        }
        else if (a == 3 && b == "C"s) {
            EXPECT_EQ(minVal, -3.9);
        }
        else if (a == 3 && b == "E"s) {
            EXPECT_EQ(minVal, 10.9);
        }
        else if (a == 3 && b == "D"s) {
            EXPECT_EQ(minVal, 1.2);
        }
        else {
            EXPECT_TRUE(false) << "must not reach here";
        }

        ++n;
    }

    EXPECT_EQ(n, expectedNumData);
}

TEST_F(HashAggregatorTests, SumTest)
{
    Metadata outputMetadata{
        {"a", tiInt},
        {"b", tiString},
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
    auto hashAggregator = makeIterator<HashAggregator>(move(mockScanner), outputMetadata, groupKeyCols, aggExprs);

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

        if (a == 1 && b == "A"s) {
            EXPECT_DOUBLE_EQ(sumVal, 92.89);
        }
        else if (a == 1 && b == "B"s) {
            EXPECT_DOUBLE_EQ(sumVal, 0.6);
        }
        else if (a == 2 && b == "C"s) {
            EXPECT_DOUBLE_EQ(sumVal, 10.1);
        }
        else if (a == 2 && b == "A"s) {
            EXPECT_DOUBLE_EQ(sumVal, 38.9);
        }
        else if (a == 3 && b == "C"s) {
            // TODO: #98 Investigate why DOUBLE_EQ(..., -0.2) fails
            EXPECT_FLOAT_EQ(sumVal, -0.2);
        }
        else if (a == 3 && b == "E"s) {
            EXPECT_DOUBLE_EQ(sumVal, 10.9);
        }
        else if (a == 3 && b == "D"s) {
            EXPECT_DOUBLE_EQ(sumVal, 1.2);
        }
        else {
            EXPECT_TRUE(false) << "must not reach here";
        }

        ++n;
    }

    EXPECT_EQ(n, expectedNumData);
}
