#include <algorithm>
#include <any>
#include <functional>
#include <variant>
#include <vector>

#include "any_visitor.h"
#include "expression.h"
#include "metadata.h"

namespace codein {

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
        auto name = std::any_cast<std::string>(n.leaf());
        for (size_t i = 0; i < metadata.size(); ++i) {
            if (metadata[i].fieldName == name) {
                return data[i];
            }
        }

        return std::any();
    },

    // OpCode::Const
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        return n.leaf();
    },

    // OpCode::Eq
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        auto [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) == rhs.eval(metadata, data)};
    },

    // OpCode::Neq
    evalNotSupported,

    // OpCode::Lt
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        auto [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) < rhs.eval(metadata, data)};
    },

    // OpCode::Lte
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        auto [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) <= rhs.eval(metadata, data)};
    },

    // OpCode::Gt
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        auto [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) > rhs.eval(metadata, data)};
    },

    // OpCode::Gte
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        auto [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) >= rhs.eval(metadata, data)};
    },

    // OpCode::Add
    [](const Expression& n, const Metadata& metadata, const std::vector<std::any>& data) {
        auto [lhs, rhs] = n.firstAndSecond();
        return std::any{lhs.eval(metadata, data) + rhs.eval(metadata, data)};
    },

    // OpCode::Sub
    evalNotSupported,

    // OpCode::Mult
    evalNotSupported,

    // OpCode::Div
    evalNotSupported,
};

std::any Expression::eval(const Metadata& metadata, const std::vector<std::any>& data) const {
    return evaluators[static_cast<size_t>(opCode)](*this, metadata, data);
}

}
