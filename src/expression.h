#include <any>
#include <string>
#include <variant>
#include <vector>

#include "metadata.h"

namespace codein {

enum class OpCode {
    Noop,
    Ref,
    Const,
    Eq,
};

struct ExpressionNode {
    std::any eval(const Metadata& metadata, const std::vector<std::any>& data) const;

    OpCode opCode;
    std::variant<std::any,
        std::vector<ExpressionNode>> leafOrChildren;
};

}