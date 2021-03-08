#include <any>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "metadata.h"

#pragma once

namespace codein {

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

    OpCode opCode;
    std::variant<std::any, std::vector<Expression>> leafOrChildren;
};

}
