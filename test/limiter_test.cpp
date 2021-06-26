/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "iterator.h"
#include "metadata.h"
#include "any_visitor.h"
#include "limiter.h"
#include "mock_scanner.h"
#include "util.h"

using namespace std;
using namespace codein;

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
    };

    vector<vector<any>> expectedFields {
        {"string1"s, 1u, -1},
        {"string2"s, 2u, -2},
        {"string3"s, 3u, -3},
        {"string4"s, 4u, -4},
    };

    auto child = makeIterator<MockScanner>(metadata, lines);
    auto limiter = makeIterator<Limiter>(std::move(child), 4);
    EXPECT_FALSE(limiter->hasNext());
    verifyIteratorOutput(expectedFields, limiter);
}

TEST (LimiterTests, GreaterLimitThanChild)
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
    auto limiter = makeIterator<Limiter>(std::move(child), 7);
    EXPECT_FALSE(limiter->hasNext());
    verifyIteratorOutput(expectedFields, limiter);
}
