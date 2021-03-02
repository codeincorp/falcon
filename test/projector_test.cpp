#include <gtest/gtest.h>
#include <string>
#include <typeindex>
#include <vector>

#include "to_any_converter.h"
#include "any_visitor.h"
#include "csv_file_scanner.h"
#include "projector.h"

using namespace std;
using namespace codein;

bool operator==(const Metadata& lhs, const Metadata& rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i].fieldName != rhs[i].fieldName || lhs[i].typeIndex != rhs[i].typeIndex) {
            return false;
        }
    }

    return true;
}

struct ProjectorTests : public ::testing::Test {
    Metadata metadata{
        {"a", tiDouble},
        {"b", tiFloat},
        {"c", tiInt},
        {"d", tiUint},
        {"e", tiString}
    };
    vector<string> lines{
        "10.1, -1.5f, -3, 2020, 3847 1ST ST SE"
    };
};

TEST_F(ProjectorTests, SanityTest)
{
    auto fields = parseLine(lines[0]);
    std::vector<std::any> expected;
    for (size_t i = 0; i < metadata.size(); ++i) {
        expected.emplace_back(convertTo(anyConverters, metadata[i].typeIndex, fields[i]));
    }

    auto scanner = makeIterator<CsvFileScanner>(metadata, lines);

    vector<string> columns{
        "a", "b", "c", "d", "e"
    };

    auto projector = makeIterator<Projector>(columns, std::move(scanner));

    projector->open();
    ASSERT_TRUE(projector->hasMore());
    ASSERT_TRUE(projector->getMetadata() == metadata);

    optional<vector<any>> actual = projector->processNext();
    ASSERT_TRUE(actual.has_value());

    const auto& val = actual.value();
    ASSERT_EQ(val.size(), metadata.size());
    for (size_t i = 0; i < val.size(); ++i) {
        EXPECT_TRUE(type_index(val[i].type()) == metadata[i].typeIndex);
        EXPECT_TRUE(val[i] == expected[i]);
    }
}

TEST_F(ProjectorTests, SelectionTest)
{
    vector<string> columns{
        "e", "c", "a"
    };
    Metadata expectedMetadata{
        {"e", tiString},
        {"c", tiInt},
        {"a", tiDouble},
    };
 
    auto fields = parseLine(lines[0]);
    std::vector<std::any> expected;
    expected.emplace_back(convertTo(anyConverters, metadata[4].typeIndex, fields[4]));
    expected.emplace_back(convertTo(anyConverters, metadata[2].typeIndex, fields[2]));
    expected.emplace_back(convertTo(anyConverters, metadata[0].typeIndex, fields[0]));

    auto scanner = makeIterator<CsvFileScanner>(metadata, lines);

    auto projector = makeIterator<Projector>(columns, std::move(scanner));

    projector->open();
    ASSERT_TRUE(projector->hasMore());
    ASSERT_TRUE(projector->getMetadata() == expectedMetadata);

    optional<vector<any>> actual = projector->processNext();
    ASSERT_TRUE(actual.has_value());

    const auto& val = actual.value();
    ASSERT_EQ(val.size(), expectedMetadata.size());
    for (size_t i = 0; i < val.size(); ++i) {
        EXPECT_TRUE(type_index(val[i].type()) == expectedMetadata[i].typeIndex);
        EXPECT_TRUE(val[i] == expected[i]);
    }
}
