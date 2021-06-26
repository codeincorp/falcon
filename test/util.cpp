/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <gtest/gtest.h>
#include <any>
#include <memory>
#include <vector>

#include "any_visitor.h"
#include "iterator.h"
#include "util.h"

using namespace std;
using namespace codein;

/**
 * @brief Compares actual data that an iterator outputs to expected data.
 * 
 * @param expectedFields: expected fields of data if iterator operates as intended.
 * @param iterator: unique_ptr of iterator that will be tested.
 */
void verifyIteratorOutput (
    const vector<vector<any>>& expectedFields, const unique_ptr<Iterator>& iterator)
{
    iterator->open();
    const size_t kExpectedPassLines = expectedFields.size();
    size_t i = 0;

    while (iterator->hasNext()) {
        auto row = iterator->processNext();

        if (row == std::nullopt) {
            break;
        }

        auto val = row.value();
        for (size_t k = 0; k < expectedFields[i].size(); ++k) {
            const auto& actualType = val[k].type();
            const auto& expectedType = expectedFields[i][k].type();
            EXPECT_TRUE(actualType == expectedType)
                << "actual type = " << actualType.name() << ", "
                << "expected type = " << expectedType.name();
            EXPECT_TRUE(val[k] == expectedFields[i][k]);
        }

        ++i;
    }

    EXPECT_FALSE(iterator->hasNext());
    EXPECT_TRUE(iterator->processNext() == std::nullopt);
    EXPECT_EQ(i, kExpectedPassLines);
}
