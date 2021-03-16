#include <any>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <vector>

#include "iterator.h"
#include "expression.h"
#include "hash_aggregator.h"

namespace codein {

Expression HashAggregator::createHashExpr(const std::vector<std::string>& groupKeyCols)
{
    Expression expr{
        .opCode = OpCode::Hash,
        .leafOrChildren = std::vector<Expression>{}
    };

    expr.children().reserve(groupKeyCols.size());
    for (const auto& col: groupKeyCols) {
        auto [found, _] = inputMetadata_.find(col);
        assert(found);
        expr.children().emplace_back(Expression{.opCode = OpCode::Ref, .leafOrChildren = std::any(col)});
    }

    return expr;
}

HashAggregator::HashAggregator(
        std::unique_ptr<Iterator>&& child,
        const Metadata& outputMetadata,
        const std::vector<std::string>& groupKeyCols,
        const std::vector<Expression>& aggExprs)
    : child_(std::move(child))
    , inputMetadata_(child_->getMetadata())
    , outputMetadata_(outputMetadata)
    , hashExpr_(createHashExpr(groupKeyCols))
    , aggExprs_(aggExprs)
{
}

std::optional<std::vector<std::any>> HashAggregator::processNext()
{
    return std::nullopt;
}

} // namespace codein;
