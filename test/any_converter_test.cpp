/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <gtest/gtest.h>
#include <climits>
#include <typeindex>

#include "to_any_converter.h"

using namespace std;
using namespace codein;

TEST(AnyConverterTests, SuccessfulCases)
{
    auto val = convertTo(tiInt, "1");
    EXPECT_EQ(type_index(val.type()), tiInt);
    EXPECT_EQ(any_cast<int>(val), 1);

    val = convertTo(tiInt, "-20");
    EXPECT_EQ(type_index(val.type()), tiInt);
    EXPECT_EQ(any_cast<signed int>(val), -20);

    val = convertTo(tiInt, "1.1");
    EXPECT_EQ(type_index(val.type()), tiInt);
    EXPECT_EQ(any_cast<int>(val), 1);

    val = convertTo(tiInt, "7abc");
    EXPECT_EQ(type_index(val.type()), tiInt);
    EXPECT_EQ(any_cast<int32_t>(val), 7);

    val = convertTo(tiUint, "20");
    EXPECT_EQ(type_index(val.type()), tiUint);
    EXPECT_EQ(any_cast<unsigned>(val), 20);

    val = convertTo(tiUint, "20.14");
    EXPECT_EQ(type_index(val.type()), tiUint);
    EXPECT_EQ(any_cast<unsigned>(val), 20);

    val = convertTo(tiUint, "101.1.4");
    EXPECT_EQ(type_index(val.type()), tiUint);
    EXPECT_EQ(any_cast<unsigned int>(val), 101);

    val = convertTo(tiFloat, "1.1");
    EXPECT_EQ(type_index(val.type()), tiFloat);
    EXPECT_FLOAT_EQ(any_cast<float>(val), 1.1);

    val = convertTo(tiFloat, "1.0E2");
    EXPECT_EQ(type_index(val.type()), tiFloat);
    EXPECT_FLOAT_EQ(any_cast<float>(val), 1.0E2);

    val = convertTo(tiFloat, "12345");
    EXPECT_EQ(type_index(val.type()), tiFloat);
    EXPECT_FLOAT_EQ(any_cast<float>(val), 12345.0);

    val = convertTo(tiFloat, ".12345");
    EXPECT_EQ(type_index(val.type()), tiFloat);
    EXPECT_FLOAT_EQ(any_cast<float>(val), 0.12345);

    val = convertTo(tiDouble, "0.00001");
    EXPECT_EQ(type_index(val.type()), tiDouble);
    EXPECT_DOUBLE_EQ(any_cast<double>(val), 0.00001);

    val = convertTo(tiDouble, "12345E-43");
    EXPECT_EQ(type_index(val.type()), tiDouble);
    EXPECT_DOUBLE_EQ(any_cast<double>(val), 12345E-43);

    val = convertTo(tiDouble, ".12345");
    EXPECT_EQ(type_index(val.type()), tiDouble);
    EXPECT_DOUBLE_EQ(any_cast<double>(val), 0.12345);

    val = convertTo(tiString, "1.1");
    EXPECT_EQ(type_index(val.type()), tiString);
    EXPECT_EQ(any_cast<string>(val), "1.1");
}

TEST(AnyConverterTests, BoundaryValues)
{
    auto val = convertTo(tiInt, to_string(INT32_MAX));
    EXPECT_EQ(type_index(val.type()), tiInt);
    EXPECT_EQ(any_cast<int>(val), INT32_MAX);

    val = convertTo(tiInt, to_string(INT32_MIN));
    EXPECT_EQ(type_index(val.type()), tiInt);
    EXPECT_EQ(any_cast<int>(val), INT32_MIN);

    val = convertTo(tiUint, to_string(UINT32_MAX));
    EXPECT_EQ(type_index(val.type()), tiUint);
    EXPECT_EQ(any_cast<unsigned>(val), UINT32_MAX);

    val = convertTo(tiFloat, to_string(FLT_MAX));
    EXPECT_EQ(type_index(val.type()), tiFloat);
    EXPECT_FLOAT_EQ(any_cast<float>(val), FLT_MAX);

    auto float_min = "1.1754944e-38";
    val = convertTo(tiFloat, float_min);
    EXPECT_EQ(type_index(val.type()), tiFloat);
    EXPECT_FLOAT_EQ(any_cast<float>(val), FLT_MIN) << "val = " << any_cast<float>(val);

    val = convertTo(tiDouble, to_string(DBL_MAX));
    EXPECT_EQ(type_index(val.type()), tiDouble);
    EXPECT_DOUBLE_EQ(any_cast<double>(val), DBL_MAX);

    auto double_min = "2.2250738585072014e-308";
    val = convertTo(tiDouble, double_min);
    EXPECT_EQ(type_index(val.type()), tiDouble);
    EXPECT_DOUBLE_EQ(any_cast<double>(val), DBL_MIN) << "val = " << any_cast<double>(val);
}

TEST(AnyConverterTests, FailureCases)
{
    auto val = convertTo(tiInt, to_string(UINT32_MAX));
    EXPECT_FALSE(val.has_value());
    EXPECT_THROW(any_cast<int>(val), bad_any_cast);

    val = convertTo(tiUint, "abc");
    EXPECT_FALSE(val.has_value());
    EXPECT_THROW(any_cast<unsigned>(val), bad_any_cast);

    /* TODO: implement this behavior
    val = convertTo(tiUint, "-1");
    EXPECT_FALSE(val.has_value());
    EXPECT_THROW(any_cast<unsigned>(val), bad_any_cast);
    */

    val = convertTo(type_index(typeid(long double)), "1");
    EXPECT_FALSE(val.has_value());
    EXPECT_THROW(any_cast<int>(val), bad_any_cast);
}
 