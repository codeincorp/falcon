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
{
    // The intermediate aggregation values are part of input values to aggregation expressions.
    for (size_t i = groupKeyCols.size(); i < outputMetadata_.size(); ++i) {
        inputMetadata_.emplace_back(outputMetadata_[i]);
    }
}

std::vector<std::any> mergeValues(std::vector<std::any>&& inputVals, std::vector<std::any>& aggVals)
{
    auto r = std::move(inputVals);
    for (auto& aggVal: aggVals) {
        r.emplace_back(std::move(aggVal));
    }

    return r;
}

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

        if (!hashStorage_.contains(groupKeyVals)) {
            hashStorage_.emplace(groupKeyVals, std::vector<std::any>(aggExprs_.size()));

            auto inputVals = mergeValues(std::move(optInput.value()), hashStorage_[groupKeyVals]);
            for (size_t i = 0; i < aggExprs_.size(); ++i) {
                hashStorage_[groupKeyVals][i] = aggExprs_[i].initExpr.eval(inputMetadata_, inputVals);
            }
        }
        else {
            auto inputVals = mergeValues(std::move(optInput.value()), hashStorage_[groupKeyVals]);
            for (size_t i = 0; i < aggExprs_.size(); ++i) {
                hashStorage_[groupKeyVals][i] = aggExprs_[i].contExpr.eval(inputMetadata_, inputVals);
            }
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
    
    return mergeValues(std::move(nh.key()), nh.mapped());
}

} // namespace codein;
