#include <any>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "iterator.h"
#include "expression.h"
#include "hash_aggregator.h"

namespace codein {

std::size_t HashAggregator::Hasher::operator()(const std::vector<std::any>& groupKeyVals) const
{
    std::size_t h = 0;
    for (const auto& keyVal: groupKeyVals) {
        h ^= hashAny(keyVal);
    }

    return h;
}

bool HashAggregator::KeyEqual::operator()(const std::vector<std::any>& lhs, const std::vector<std::any>& rhs) const
{
    std::size_t n = lhs.size();
    if (n != rhs.size()) {
        return false;
    }

    for (std::size_t i = 0; i < n; ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
}

std::vector<Expression> HashAggregator::createGroupKeyProjExprs(
    const Metadata& inputMetadata, const std::vector<std::string>& groupKeyCols)
{
    std::vector<Expression> groupKeyProjExprs;
    groupKeyProjExprs.reserve(groupKeyCols.size());

    for (const auto& col: groupKeyCols) {
        auto [found, _] = inputMetadata.find(col);
        assert(found);
        groupKeyProjExprs.emplace_back(Expression{.opCode = OpCode::Ref, .leafOrChildren = std::any(col)});
    }

    return groupKeyProjExprs;
}

HashAggregator::HashAggregator(
    std::unique_ptr<Iterator>&& child,
    const Metadata& outputMetadata,
    const std::vector<std::string>& groupKeyCols,
    const std::vector<AggregationExpression>& aggExprs)
    : child_(std::move(child))
    , inputMetadata_(child_->getMetadata())
    , outputMetadata_(outputMetadata)
    , groupKeyProjExprs_(createGroupKeyProjExprs(inputMetadata_, groupKeyCols))
    , aggExprs_(aggExprs)
    , hashStorage_(1023)
{}

void HashAggregator::open()
{
    child_->open();

    while (child_->hasMore()) {
        auto optInput = child_->processNext();
        if (!optInput) {
            return;
        }

        std::vector<std::any> groupKeyVals;
        groupKeyVals.reserve(groupKeyProjExprs_.size());

        for (const auto& proj : groupKeyProjExprs_) {
            groupKeyVals.emplace_back(proj.eval(inputMetadata_, optInput.value()));
        }

        if (auto it = hashStorage_.find(groupKeyVals); it == hashStorage_.cend()) {
            hashStorage_.emplace(std::move(groupKeyVals), std::vector<std::any>(aggExprs_.size()));

            for (const auto& [initExpr, _] : aggExprs_) {
                initExpr.eval(inputMetadata_, optInput.value(), hashStorage_);
            }

        }
        else {
            it->second
        }
    }

    it_ = hashStorage_.cbegin();
}

std::optional<std::vector<std::any>> HashAggregator::processNext()
{
    if (it_ == hashStorage_.cend()) {
        return std::nullopt;
    }

    auto cit = it_++;
    auto nh = hashStorage_.extract(cit);
    
    return std::move(nh.key());
}

} // namespace codein;
