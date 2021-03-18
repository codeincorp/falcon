#include <algorithm>
#include <any>
#include <functional>
#include <variant>
#include <vector>

#include "any_visitor.h"
#include "expression.h"
#include "metadata.h"

namespace codein {

const Expression kAlwaysTrue{.opCode = OpCode::Const, .leafOrChildren = std::any(true)};

const Expression kAlwaysFalse{.opCode = OpCode::Const, .leafOrChildren = std::any(false)};

using Evaluator = std::function<std::any (const Expression&, const Metadata&, const std::vector<std::any>&)>;

const Evaluator evalNoop = [](const Expression&, const Metadata&, const std::vector<std::any>&) {
    assert(!"Nothing to evaluate for Noop");
    return std::any();
};

const Evaluator evalNotSupported = [](const Expression&, const Metadata&, const std::vector<std::any>&) {
    assert(!"Not supported");
    return std::any();
};

const std::vector<Evaluator> evaluators{
    // OpCode::Noop
    evalNoop,

    // OpCode::Ref
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        const auto& name = std::any_cast<std::string>(n.leaf());
        return data[metadata[name]];
    },

    // OpCode::Const
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        return n.leaf();
    },

    // OpCode::Eq
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        const auto& [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) == rhs.eval(metadata, data)};
    },

    // OpCode::Neq
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        const auto& [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) != rhs.eval(metadata, data)};
    },

    // OpCode::Lt
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        const auto& [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) < rhs.eval(metadata, data)};
    },

    // OpCode::Lte
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        const auto& [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) <= rhs.eval(metadata, data)};
    },

    // OpCode::Gt
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        const auto& [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) > rhs.eval(metadata, data)};
    },

    // OpCode::Gte
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        const auto& [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) >= rhs.eval(metadata, data)};
    },

    // OpCode::Add
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        const auto& [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) + rhs.eval(metadata, data)};
    },

    // OpCode::Sub
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        const auto& [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) - rhs.eval(metadata, data)};
    },

    // OpCode::Mult
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        const auto& [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) * rhs.eval(metadata, data)};
    },

    // OpCode::Div
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        const auto& [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) / rhs.eval(metadata, data)};
    },

    // OpCode::Mod
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        const auto& [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) % rhs.eval(metadata, data)};
    },

    // OpCode::Not
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        auto lhs = n.first();
        return std::any{notAny(lhs.eval(metadata, data))};
    },

    // OpCode::And
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        auto [lhs, rhs] = n.firstAndSecond();
        // Implements short-circuit.
        if (auto r = lhs.eval(metadata, data); std::any_cast<bool>(r)) {
            return rhs.eval(metadata, data);
        }
        else {
            return std::any(false);
        }
    },

    // OpCode::Or
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        auto [lhs, rhs] = n.firstAndSecond();
        // Implements short-circuit.
        if (auto r = lhs.eval(metadata, data); std::any_cast<bool>(r)) {
            return std::any(true);
        }
        else {
            return rhs.eval(metadata, data);
        }
    },

    // OpCode::Assign
};

std::any Expression::eval(const Metadata& metadata, const std::vector<std::any>& data) const {
    return evaluators[static_cast<size_t>(opCode)](*this, metadata, data);
}

}
