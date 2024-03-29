/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <any>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "metadata.h"

#pragma once

namespace codein {

/**
 * @brief Exception for when identifier name expected for OpCode::Ref but
 * identifier does not exist.
 */
class NameExpected {};

enum class OpCode {
    Noop,
    Ref,
    Const,
    Eq,
    Neq,
    Lt,
    Lte,
    Gt,
    Gte,
    Add,
    Sub,
    Mult,
    Div,
    Mod,
    Not,
    And,
    Or,
    Cond,
    Conv,
};

struct Expression {
    const std::any& leaf() const {
        const auto& leaf = std::get<0>(leafOrChildren);
        return leaf;
    }

    std::any& leaf() {
        auto& leaf = std::get<0>(leafOrChildren);
        return leaf;
    }

    const std::vector<Expression>& children() const {
        const auto& children = std::get<1>(leafOrChildren);
        return children;
    }

    std::vector<Expression>& children() {
        auto& children = std::get<1>(leafOrChildren);
        return children;
    }

    const Expression& first() const {
        const auto& first = children()[0];
        return first;
    }

    Expression& first() {
        auto& first = children()[0];
        return first;
    }

    const Expression& second() const {
        const auto& second = children()[1];
        return second;
    }

    Expression& second() {
        auto& second = children()[1];
        return second;
    }

    auto firstAndSecond() const {
        const auto& c = children();
        const auto& first = c[0];
        const auto& second = c[1];
        return std::make_tuple(first, second);
    }

    auto firstAndSecond() {
        auto& c = children();
        auto& first = c[0];
        auto& second = c[1];
        return std::make_tuple(first, second);
    }

    std::any eval(const Metadata& metadata, const std::vector<std::any>& data) const;
    std::any operator()(const Metadata& metadata, const std::vector<std::any>& data) const
    {
        return eval(metadata, data);
    }

    OpCode opCode;
    std::variant<std::any, std::vector<Expression>> leafOrChildren;
};

extern const Expression kAlwaysTrue;

extern const Expression kAlwaysFalse;

}
