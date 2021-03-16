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
    const std::vector<Expression>& aggExprs)
    : child_(std::move(child))
    , inputMetadata_(child_->getMetadata())
    , outputMetadata_(outputMetadata)
    , groupKeyProjExprs_(createGroupKeyProjExprs(inputMetadata_, groupKeyCols))
    , aggExprs_(aggExprs)
    , hashStorage_(1023, Hasher(), KeyEqual())
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

        if (!hashStorage_.contains(groupKeyVals)) {
            hashStorage_.emplace(std::move(groupKeyVals), 0);
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
