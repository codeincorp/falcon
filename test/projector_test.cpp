#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <typeindex>
#include <vector>

#include "metadata.h"
#include "to_any_converter.h"
#include "any_visitor.h"
#include "csv_file_scanner.h"
#include "projector.h"

using namespace std;
using namespace codein;

struct ProjectorTests : public ::testing::Test {
    const string metadataFileName = "projector.txt";
    const string dataFileName = "projector.csv";
    Metadata metadata;
    vector<string> lines;

    void SetUp() override
    {
        fstream mfs(metadataFileName);
        string line;

        if (!mfs.fail()) {
            getline(mfs, line);
            metadata = parseLineMetadata(line);
        }

        fstream dfs(dataFileName);
        if (!dfs.fail()) {
            getline(dfs, line);
            lines.emplace_back(line);
        }
    }
};

TEST_F(ProjectorTests, SanityTest)
{
    vector<string> fields = parseLine(lines[0]);
    std::vector<std::any> expected;
    for (size_t i = 0; i < metadata.size(); ++i) {
        expected.emplace_back(convertTo(anyConverters, metadata[i].typeIndex, fields[i]));
    }

    auto scanner = makeIterator<CsvFileScanner>(metadataFileName, dataFileName);

    vector<string> columns{
        "a", "b", "c", "d", "e"
    };

    auto projector = makeIterator<Projector>(columns, std::move(scanner));

    projector->open();
    ASSERT_TRUE(projector->hasMore());
    ASSERT_TRUE(projector->getMetadata() == metadata);

    optional<vector<any>> actual = projector->processNext();
    ASSERT_TRUE(actual.has_value());

    const vector<any>& val = actual.value();
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

    auto scanner = makeIterator<CsvFileScanner>(metadataFileName, dataFileName);

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
