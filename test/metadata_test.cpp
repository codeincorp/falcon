/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <gtest/gtest.h>

#include "metadata.h"

using namespace std;
using namespace codein;

TEST(MetadataTests, SanityTest)
{
    // Verifies that default ctor works.
    Metadata meta0;

    EXPECT_EQ(meta0.size(), 0);

    // Adds two fields.
    meta0.emplace_back("a", tiInt);
    meta0.emplace_back("b", tiUint);

    // Verifies that emplace_back() works.
    EXPECT_EQ(meta0.size(), 2);
    EXPECT_EQ(meta0[0].fieldName, "a");
    EXPECT_EQ(meta0[0].typeIndex, tiInt);
    EXPECT_EQ(meta0[1].fieldName, "b");
    EXPECT_EQ(meta0[1].typeIndex, tiUint);
}

TEST(MetadataTests, CtorsAndAssignmentsTest)
{
    // Verifies that initializer_list ctor works.
    Metadata meta0{
        {"c", tiFloat},
        {"d", tiString},
        {"e", tiDouble},
    };

    EXPECT_EQ(meta0.size(), 3);
    EXPECT_EQ(meta0[0].fieldName, "c");
    EXPECT_EQ(meta0[0].typeIndex, tiFloat);
    EXPECT_EQ(meta0[1].fieldName, "d");
    EXPECT_EQ(meta0[1].typeIndex, tiString);
    EXPECT_EQ(meta0[2].fieldName, "e");
    EXPECT_EQ(meta0[2].typeIndex, tiDouble);

    // Verifies that copy ctor works.
    Metadata meta1(meta0);

    EXPECT_TRUE(meta0 == meta1);

    // Verifies that move ctor works.
    Metadata meta2(move(meta1));

    EXPECT_TRUE(meta0 == meta2);
    EXPECT_EQ(meta1.size(), 0);

    // Verifies that copy assignment works.
    meta1 = meta2;

    EXPECT_TRUE(meta1 == meta2);

    // Verifies that move assignment works.
    Metadata meta3;
    meta3 = move(meta1);

    EXPECT_TRUE(meta3 == meta2);
    EXPECT_EQ(meta1.size(), 0);

    // Verifies that [size_t] returns reference.
    meta3[0].fieldName = "k";
    EXPECT_EQ(meta3[0].fieldName, "k");
}

TEST(MetadataTests, IndexingTest)
{
    Metadata meta0{
        {"a", tiInt},
        {"b", tiUint},
        {"c", tiFloat},
        {"d", tiString},
        {"e", tiDouble},
    };

    // Verifies that indexing works after construction.
    EXPECT_EQ(meta0["c"], 2);
    EXPECT_EQ(meta0["e"], 4);
    EXPECT_EQ(meta0["a"], 0);
    EXPECT_EQ(meta0["b"], 1);
    EXPECT_EQ(meta0["d"], 3);

    // Verifies that indexing works after emplace_back.
    meta0.emplace_back("f", tiUint);
    EXPECT_EQ(meta0["f"], 5);
    EXPECT_EQ(meta0["c"], 2);
    EXPECT_EQ(meta0["e"], 4);
    EXPECT_EQ(meta0["a"], 0);
    EXPECT_EQ(meta0["b"], 1);
    EXPECT_EQ(meta0["d"], 3);

    meta0.emplace_back("g", tiUint);
    EXPECT_EQ(meta0["f"], 5);
    EXPECT_EQ(meta0["c"], 2);
    EXPECT_EQ(meta0["e"], 4);
    EXPECT_EQ(meta0["g"], 6);
    EXPECT_EQ(meta0["a"], 0);
    EXPECT_EQ(meta0["b"], 1);
    EXPECT_EQ(meta0["d"], 3);
}

TEST(MetadataTests, InvalidIndexingTest)
{
    Metadata meta0{
        {"a", tiInt},
        {"b", tiUint},
        {"c", tiFloat},
        {"d", tiString},
        {"e", tiDouble},
    };

    EXPECT_THROW(meta0["k"], UnknownName);
}
