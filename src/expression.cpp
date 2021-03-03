#include <algorithm>
#include <any>
#include <variant>
#include <vector>

#include "any_visitor.h"
#include "expression.h"
#include "metadata.h"

namespace codein {

std::any ExpressionNode::eval(const Metadata& metadata, const std::vector<std::any>& data) {
    switch (opCode) {
    case OpCode::Ref: {
        auto name = any_cast<std::string>(std::get<0>(leafOrChildren));
        for (size_t i = 0; i < metadata.size(); ++i) {
            if (metadata[i].fieldName == name) {
                return data[i];
            }
        }
        break;
    }
    
    case OpCode::Const:
        return std::get<0>(leafOrChildren);
        break;
    
    case OpCode::Eq: {
        auto children = std::get<1>(leafOrChildren);
        auto lhs = children[0];
        auto rhs = children[1];
        return {lhs.eval(metadata, data) == rhs.eval(metadata, data)};
        break;
    }

    default:
        break;
    }

    assert(!"Unsupported op");
    return std::any();
}

}
