/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <typeindex>
#include <vector>

#include "any_visitor.h"
#include "csv_file_scanner.h"
#include "expression.h"
#include "metadata.h"
#include "projector.h"
#include "to_any_converter.h"

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
        expected.emplace_back(convertTo(metadata[i].typeIndex, fields[i]));
    }

    auto scanner = makeIterator<CsvFileScanner>(metadataFileName, dataFileName);

    vector<Expression> projections{
        {.opCode = OpCode::Ref, .leafOrChildren = any("a"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = any("b"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = any("c"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = any("d"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = any("e"s)},
    };

    auto projector = makeIterator<Projector>(std::move(scanner), projections, metadata);

    projector->open();
    ASSERT_TRUE(projector->hasNext());
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
    vector<Expression> projections{
        {.opCode = OpCode::Ref, .leafOrChildren = any("e"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = any("c"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = any("a"s)},
    };

    Metadata expectedMetadata{
        {"e", tiString},
        {"c", tiInt},
        {"a", tiDouble},
    };
 
    auto fields = parseLine(lines[0]);
    std::vector<std::any> expected;
    expected.emplace_back(convertTo(metadata[4].typeIndex, fields[4]));
    expected.emplace_back(convertTo(metadata[2].typeIndex, fields[2]));
    expected.emplace_back(convertTo(metadata[0].typeIndex, fields[0]));

    auto scanner = makeIterator<CsvFileScanner>(metadataFileName, dataFileName);

    auto projector = makeIterator<Projector>(std::move(scanner), projections, expectedMetadata);

    projector->open();
    ASSERT_TRUE(projector->hasNext());
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

TEST_F(ProjectorTests, ProjExprTest)
{
    vector<Expression> projections{
        // e + " LA"
        {
            .opCode = OpCode::Add,
            .leafOrChildren = vector<Expression>{
                {.opCode = OpCode::Ref, .leafOrChildren = any("e"s)},
                {.opCode = OpCode::Const, .leafOrChildren = any(" LA"s)},
            }
        },
        // c == -3
        {
            .opCode = OpCode::Eq,
            .leafOrChildren = vector<Expression>{
                {.opCode = OpCode::Ref, .leafOrChildren = any("c"s)},
                {.opCode = OpCode::Const, .leafOrChildren = any(-3)},
            }
        },
        // a * 3.0
        {
            .opCode = OpCode::Mult,
            .leafOrChildren = vector<Expression>{
                {.opCode = OpCode::Ref, .leafOrChildren = any("a"s)},
                {.opCode = OpCode::Const, .leafOrChildren = any(3.0)},
            }
        }
    };

    Metadata expectedMetadata{
        {"e", tiString},
        {"c", tiBool},
        {"a", tiDouble},
    };
 
    auto fields = parseLine(lines[0]);
    std::vector<std::any> expected;
    expected.emplace_back(convertTo(metadata[4].typeIndex, fields[4]) + any(" LA"s));
    expected.emplace_back(convertTo(metadata[2].typeIndex, fields[2]) == any(-3));
    expected.emplace_back(convertTo(metadata[0].typeIndex, fields[0]) * any(3.0));

    auto scanner = makeIterator<CsvFileScanner>(metadataFileName, dataFileName);

    auto projector = makeIterator<Projector>(std::move(scanner), projections, expectedMetadata);

    projector->open();
    ASSERT_TRUE(projector->hasNext());
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
