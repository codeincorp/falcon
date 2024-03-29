/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <gtest/gtest.h>
#include <memory>

#include "filter.h"
#include "iterator.h"
#include "mock_scanner.h"

using namespace std;
using namespace codein;

struct FilterTests : public ::testing::Test {
    Metadata metadata{
        { "a", tiInt },
        { "b", tiFloat },
        { "c", tiString },
    };

    vector<string> lines{
        "1,1.1,John Smith",
        "2,2.2,Alex Smith",
        "3,3.3,Alex Swanson",
    };
};

TEST_F(FilterTests, BasicTest)
{
    Expression filterExpr{
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(2)},
        }
    };

    auto mockScanner = makeIterator<MockScanner>(metadata, lines);

    auto filter = makeIterator<Filter>(move(mockScanner), filterExpr);

    filter->open();
    EXPECT_TRUE(filter->hasNext());

    auto data = filter->processNext();
    EXPECT_TRUE(data.has_value());

    const auto& val = data.value();
    EXPECT_EQ(any_cast<int>(val[0]), 2);
    EXPECT_EQ(any_cast<float>(val[1]), 2.2f);
    EXPECT_EQ(any_cast<string>(val[2]), "Alex Smith"s);

    EXPECT_TRUE(filter->hasNext());

    data = filter->processNext();
    EXPECT_FALSE(data.has_value());
}

TEST_F(FilterTests, PassThruTest)
{
    Expression filterExpr{
        .opCode = OpCode::Const,
        .leafOrChildren = std::any(true)
    };

    auto mockScanner = makeIterator<MockScanner>(metadata, lines);

    auto filter = makeIterator<Filter>(move(mockScanner), filterExpr);

    filter->open();
    EXPECT_TRUE(filter->hasNext());

    auto data = filter->processNext();
    EXPECT_TRUE(data.has_value());

    const auto& val = data.value();
    EXPECT_EQ(any_cast<int>(val[0]), 1);
    EXPECT_EQ(any_cast<float>(val[1]), 1.1f);
    EXPECT_EQ(any_cast<string>(val[2]), "John Smith"s);

    EXPECT_TRUE(filter->hasNext());
    data = filter->processNext();
    EXPECT_TRUE(data.has_value());

    const auto& val2 = data.value();
    EXPECT_EQ(any_cast<int>(val2[0]), 2);
    EXPECT_EQ(any_cast<float>(val2[1]), 2.2f);
    EXPECT_EQ(any_cast<string>(val2[2]), "Alex Smith"s);

    EXPECT_TRUE(filter->hasNext());
    data = filter->processNext();

    const auto& val3 = data.value();
    EXPECT_EQ(any_cast<int>(val3[0]), 3);
    EXPECT_EQ(any_cast<float>(val3[1]), 3.3f);
    EXPECT_EQ(any_cast<string>(val3[2]), "Alex Swanson"s);
}

TEST_F(FilterTests, NoDataTest)
{
    Expression filterExpr{
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(0)},
        }
    };

    auto mockScanner = makeIterator<MockScanner>(metadata, lines);

    auto filter = makeIterator<Filter>(move(mockScanner), filterExpr);

    filter->open();
    EXPECT_TRUE(filter->hasNext());

    auto data = filter->processNext();
    EXPECT_FALSE(data.has_value());
    EXPECT_FALSE(filter->hasNext());
}

TEST_F(FilterTests, InvalidFilterTest)
{
    Expression filterExpr{
        .opCode = OpCode::Noop,
        .leafOrChildren = std::any()
    };

    auto mockScanner = makeIterator<MockScanner>(metadata, lines);

    EXPECT_THROW(makeIterator<Filter>(move(mockScanner), filterExpr), InvalidFilter);
}
