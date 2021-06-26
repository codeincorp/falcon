/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <any>
#include <gtest/gtest.h>
#include <vector>

#include "any_visitor.h"
#include "expression.h"
#include "metadata.h"

using namespace std;
using namespace codein;

TEST(ConversionTests, BoolConvTest)
{
    // conv(true, "int") == 1
    Expression expr{
        .opCode = OpCode::Conv,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Const, .leafOrChildren = any(true)},
            {.opCode = OpCode::Const, .leafOrChildren = any("int"s)},
        }
    };
    Metadata metadata{};
    vector<any> data{};

    auto r = expr(metadata, data);
    EXPECT_EQ(any_cast<int>(r), 1);

    // conv(false, "int") == 0
    expr.first() = {OpCode::Const, any(false)};
    EXPECT_EQ(any_cast<int>(expr(metadata, data)), 0);

    // conv(false, "unsigned") == 0u
    expr.second().leafOrChildren = any("unsigned"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<unsigned>(r), 0u);

    // conv(true, "unsigned") == 1u
    expr.first() = {OpCode::Const, any(true)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<unsigned>(r), 1u);

    // conv(true, "float") == 1.0
    expr.second().leafOrChildren = any("float"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<float>(r), 1.0f);

    // conv(false, "float") == 0.0
    expr.first() = {OpCode::Const, any(false)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<float>(r), 0.0f);

    // conv(false, "double") == 0.0
    expr.second().leafOrChildren = any("double"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<double>(r), 0.0);

    // conv(true, "double") == 1.0
    expr.first() = {OpCode::Const, any(true)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<double>(r), 1.0);

    // conv(true, "string") == "1"
    expr.second().leafOrChildren = any("string"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<string>(r), "1"s);

    // conv(false, "string") == "0"
    expr.first() = {OpCode::Const, any(false)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<string>(r), "0"s);
}

TEST(ConversionTests, IntConvTest)
{
    // conv(3, "unsigned") 
    Expression expr{
        .opCode = OpCode::Conv,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Const, .leafOrChildren = any(3)},
            {.opCode = OpCode::Const, .leafOrChildren = any("unsigned"s)},
        }
    };
    Metadata metadata{};
    vector<any> data{};

    auto r = expr(metadata, data);
    EXPECT_EQ(any_cast<unsigned>(r), 3u);

    // conv(-3, "unsigned") == unsigned(-3)
    expr.first() = {OpCode::Const, any(-3)};
    EXPECT_EQ(any_cast<unsigned>(expr(metadata, data)), unsigned(-3));

    // conv(-3, "bool") == true
    expr.second().leafOrChildren = any("bool"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<bool>(r), true);

    // conv(0, "bool") == false
    expr.first() = {OpCode::Const, any(0)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<bool>(r), false);

    // conv(0, "float") == 0.0f
    expr.second().leafOrChildren = any("float"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<float>(r), 0.0f);

    // conv(-3, "float") == -3.0f
    expr.first() = {OpCode::Const, any(-3)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<float>(r), -3.0f);

    // conv(-3, "double") == -3.0
    expr.second().leafOrChildren = any("double"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<double>(r), -3.0);

    // conv(3, "double") == 3.0
    expr.first() = {OpCode::Const, any(3)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<double>(r), 3.0);

    // conv(3, "string") == "3"
    expr.second().leafOrChildren = any("string"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<string>(r), "3"s);

    // conv(-3, "string") == "-3"
    expr.first() = {OpCode::Const, any(-3)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<string>(r), "-3"s);
}

TEST(ConversionTests, UintConvTest)
{
    // conv(3u, "int")  == 3
    Expression expr{
        .opCode = OpCode::Conv,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Const, .leafOrChildren = any(3u)},
            {.opCode = OpCode::Const, .leafOrChildren = any("int"s)},
        }
    };
    Metadata metadata{};
    vector<any> data{};

    auto r = expr(metadata, data);
    EXPECT_EQ(any_cast<int>(r), 3);

    // conv(unsigned(-3), "int") == -3
    expr.first() = {OpCode::Const, any(unsigned(-3))};
    EXPECT_EQ(any_cast<int>(expr(metadata, data)), -3);

    // conv(unsigned(-3), "bool") == true
    expr.second().leafOrChildren = any("bool"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<bool>(r), true);

    // conv(0u, "bool") == false
    expr.first() = {OpCode::Const, any(0u)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<bool>(r), false);

    // conv(0u, "float") == 0.0f
    expr.second().leafOrChildren = any("float"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<float>(r), 0.0f);

    // conv(3u, "float") == 3.0f
    expr.first() = {OpCode::Const, any(3u)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<float>(r), 3.0f);

    // conv(3u, "double") == 3.0
    expr.second().leafOrChildren = any("double"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<double>(r), 3.0);

    // conv(0u, "double") == 0.0
    expr.first() = {OpCode::Const, any(0u)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<double>(r), 0.0);

    // conv(0u, "string") == "0"
    expr.second().leafOrChildren = any("string"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<string>(r), "0"s);

    // conv(3u, "string") == "3"
    expr.first() = {OpCode::Const, any(3u)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<string>(r), "3"s);
}

TEST(ConversionTests, FloatConvTest)
{
    // conv(3.1f, "int") == 3
    Expression expr{
        .opCode = OpCode::Conv,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Const, .leafOrChildren = any(3.1f)},
            {.opCode = OpCode::Const, .leafOrChildren = any("int"s)},
        }
    };
    Metadata metadata{};
    vector<any> data{};

    auto r = expr(metadata, data);
    EXPECT_EQ(any_cast<int>(r), 3);

    // conv(-3.1), "int") == -3
    expr.first() = {OpCode::Const, any(-3.1f)};
    EXPECT_EQ(any_cast<int>(expr(metadata, data)), -3);

    // conv(-3.1f, "bool") == true
    expr.second().leafOrChildren = any("bool"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<bool>(r), true);

    // conv(0.0f, "bool") == false
    expr.first() = {OpCode::Const, any(0.0f)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<bool>(r), false);

    // conv(0.0f, "unsigned") == 0u
    expr.second().leafOrChildren = any("unsigned"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<unsigned>(r), 0u);

    // conv(3.1f, "unsigned") == 3u
    expr.first() = {OpCode::Const, any(3.1f)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<unsigned>(r), 3u);

    // conv(3.1f, "double") == 3.1
    expr.second().leafOrChildren = any("double"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<double>(r), double(3.1f));

    // conv(-3.1f, "double") == -3.1
    expr.first() = {OpCode::Const, any(-3.1f)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<double>(r), double(-3.1f));

    // conv(-3.1f, "string") == "-3.100000"
    expr.second().leafOrChildren = any("string"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<string>(r), "-3.100000"s);

    // conv(3.1f, "string") == "3.100000"
    expr.first() = {OpCode::Const, any(3.1f)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<string>(r), "3.100000"s);
}

TEST(ConversionTests, DoubleConvTest)
{
    // conv(3.1, "int") == 3
    Expression expr{
        .opCode = OpCode::Conv,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Const, .leafOrChildren = any(3.1)},
            {.opCode = OpCode::Const, .leafOrChildren = any("int"s)},
        }
    };
    Metadata metadata{};
    vector<any> data{};

    auto r = expr(metadata, data);
    EXPECT_EQ(any_cast<int>(r), 3);

    // conv(-3.1), "int") == -3
    expr.first() = {OpCode::Const, any(-3.1)};
    EXPECT_EQ(any_cast<int>(expr(metadata, data)), -3);

    // conv(-3.1, "bool") == true
    expr.second().leafOrChildren = any("bool"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<bool>(r), true);

    // conv(0.0, "bool") == false
    expr.first() = {OpCode::Const, any(0.0)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<bool>(r), false);

    // conv(0.0, "unsigned") == 0u
    expr.second().leafOrChildren = any("unsigned"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<unsigned>(r), 0u);

    // conv(3.1, "unsigned") == 3u
    expr.first() = {OpCode::Const, any(3.1)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<unsigned>(r), 3u);

    // conv(3.1, "float") == 3.1f
    expr.second().leafOrChildren = any("float"s);
    r = expr(metadata, data);
    EXPECT_FLOAT_EQ(any_cast<float>(r), float(3.1));

    // conv(-3.1, "float") == 0.0
    expr.first() = {OpCode::Const, any(-3.1)};
    r = expr(metadata, data);
    EXPECT_FLOAT_EQ(any_cast<float>(r), float(-3.1));

    // conv(-3.1, "string") == "-3.1"
    expr.second().leafOrChildren = any("string"s);
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<string>(r), "-3.100000"s);

    // conv(3.1, "string") == "3.1"
    expr.first() = {OpCode::Const, any(3.1)};
    r = expr(metadata, data);
    EXPECT_EQ(any_cast<string>(r), "3.100000"s);
}
