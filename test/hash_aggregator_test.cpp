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
        { "c", tiString },
    };

    vector<string> lines{
        "1,A,John Smith",
        "1,A,John Smith Jr",
        "1,A,John Smith Jr II",
        "1,A,John Smith Jr III",    // group #1
        "1,B,Name 1",
        "1,B,Name 2",               // group #2
        "2,C,Name 3",
        "2,C,Name 4",
        "2,C,Name 5",               // group #3
        "2,A,Name 6",               // group #4
        "3,C,Name 7",
        "3,C,Name 8",               // group #5
        "3,E,Name 9",               // group #6
        "3,D,Name 10",              // group #7
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
