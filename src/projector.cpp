/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <any>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <vector>

#include "iterator.h"
#include "projector.h"

namespace codein {

Projector::Projector(std::unique_ptr<Iterator>&& child, const std::vector<Expression>& projections, Metadata metadata)
    : child_(std::move(child))
    , inputMetadata_(child_->getMetadata())
    , projections_(projections)
    , outputMetadata_(std::move(metadata))
{}

std::optional<std::vector<std::any>> Projector::processNext()
{
    if (!child_->hasNext()) {
        return std::nullopt;
    }

    auto input = child_->processNext();
    if (!input.has_value()) {
        return std::nullopt;
    }

    auto& inputValue = input.value();
    std::vector<std::any> output;
    output.reserve(outputMetadata_.size());
    for (size_t i = 0; i < outputMetadata_.size(); ++i) {
        output.emplace_back(std::move(projections_[i].eval(inputMetadata_, inputValue)));
    }

    return { output };
}

} // namespace codein;
