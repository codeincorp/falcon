#include <any>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <vector>

#include "iterator.h"
#include "projector.h"

namespace codein {

Projector::Projector(const std::vector<std::string>& columns, std::unique_ptr<Iterator>&& child)
    : child_(std::move(child))
    , outputMetadata_(columns.size())
    , colBinds_(columns.size())
{
    const Metadata& inputMetadata = child_->getMetadata();
    std::unordered_map<std::string, size_t> colToInputMap;
    for (size_t i = 0; i < inputMetadata.size(); ++i) {
        colToInputMap.insert(std::make_pair(inputMetadata[i].fieldName, i));
    }

    for (size_t i = 0; i < columns.size(); ++i) {
        colBinds_[i] = colToInputMap[columns[i]];
        outputMetadata_[i] = inputMetadata[colBinds_[i]];
    }
}

std::optional<std::vector<std::any>> Projector::processNext()
{
    if (!child_->hasMore()) {
        return std::nullopt;
    }

    auto input = child_->processNext();

    const auto& inputValue = input.value();
    std::vector<std::any> output(outputMetadata_.size());
    for (size_t i = 0; i < output.size(); ++i) {
        output[i] = std::move(inputValue[colBinds_[i]]);;
    }

    return {output};
}

} // namespace codein;
