#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "iterator.h"
#include "metadata.h"
#include "any_visitor.h"
#include "limiter.h"
#include "mock_scanner.h"

using namespace std;
using namespace codein;

void verifyScannerOutput1(const vector<vector<any>>& expectedFields, const unique_ptr<Iterator>& scanner) 
{
    scanner->open();
    const size_t kExpectedPassLines = expectedFields.size();
    Metadata expectedMetadata = scanner->getMetadata();
    size_t i = 0;

    while (scanner->hasMore()) {
        auto row = scanner->processNext();
        
        auto val = row.value();
        for (size_t k = 0; k < expectedMetadata.size(); ++k) {
            const auto& actualType = val[k].type();
            const auto& expectedType = expectedFields[i][k].type();
            EXPECT_TRUE(actualType == expectedType)
                << "actual type = " << actualType.name() << ", "
                << "expected type = " << expectedType.name();
            EXPECT_TRUE(val[k] == expectedFields[i][k]);
        }

        ++i;
    }

    EXPECT_FALSE(scanner->hasMore());
    EXPECT_TRUE(scanner->processNext() == std::nullopt);
    EXPECT_EQ(i, kExpectedPassLines);
}

TEST (LimiterTests, BasicTest)
{
    Metadata metadata {
        {"a", tiString}, {"b", tiUint}, {"c", tiInt}
    };

    vector<string> lines {
        "string1, 1, -1",
        "string2, 2, -2",
        "string3, 3, -3",
        "string4, 4, -4",
        "string5, 5, -5",
        "string6, 6, -6",
        "string7, 7, -7",
        "string8, 8, -8",
        "string9, 9, -9",
        "string10, 10, -10",
        "string11, 11, -11",
        "string12, 12, -12",
    };

    vector<vector<any>> expectedFields {
        {"string1"s, 1u, -1},
        {"string2"s, 2u, -2},
        {"string3"s, 3u, -3},
        {"string4"s, 4u, -4},
        {"string5"s, 5u, -5},
        {"string6"s, 6u, -6},
        {"string7"s, 7u, -7},
        {"string8"s, 8u, -8},
        {"string9"s, 9u, -9},
    };

    auto child = makeIterator<MockScanner>(metadata, lines);
    auto scanner = makeIterator<Limiter>(std::move(child), 9);
    EXPECT_FALSE(scanner->hasMore());
    verifyScannerOutput1(expectedFields, scanner);
}

TEST (LimiterTests, moreLimitThanChild)
{
    Metadata metadata {
        {"a", tiString}, {"b", tiUint}
    };

    vector<string> lines {
        "string1, 1",
        "string2, 2",
        "string3, 3",
        "string4, 4",
    };

    vector<vector<any>> expectedFields {
        {"string1"s, 1u},
        {"string2"s, 2u},
        {"string3"s, 3u},
        {"string4"s, 4u},
    };

    auto child = makeIterator<MockScanner>(metadata, lines);
    auto scanner = makeIterator<Limiter>(std::move(child), 7);

    scanner->open();
    size_t i = 0;
    while (scanner->hasMore()) {
        auto row = scanner->processNext();
        ++i;
    }
    EXPECT_GE(7, i);
    EXPECT_FALSE(scanner->hasMore());
}
