#include <gtest/gtest.h>
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
        "1,A,John Smith",
        "1,B,John Smith",
        "1,B,John Smith",
        "2,A,Alex Smith",
        "2,B,Alex Smith",
        "2,B,Alex Smith",
        "2,A,Alex Smith",
        "3,C,Alex Swanson",
        "3,D,Alex Swanson",
        "3,E,Alex Swanson",
        "3,A,Alex Swanson",
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
    vector<AggregationExpression> aggExprs{
        AggregationExpression{
            .initExpr = {
                .opCode = OpCode::Const,
                .leafOrChildren = std::any(1u),
            },
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
    const size_t expectedNumData = 8;
    while (hashAggregator->hasMore()) {
        auto optData = hashAggregator->processNext();
        if (!optData) {
            break;
        }

        ++n;
    }

    EXPECT_EQ(n, expectedNumData);
}
