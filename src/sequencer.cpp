/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include "sequencer.h"

namespace codein {

void Sequencer::checkMetadata(const std::vector<std::unique_ptr<Iterator>>& children) 
{
    Metadata metadata = children[0]->getMetadata();
    for (size_t i = 1; i < children.size(); ++i) {
        if (children[i]->getMetadata() != metadata) {
            throw DiscrepantOutputData();
        }
    }
}

Sequencer::Sequencer(std::vector<std::unique_ptr<Iterator>>&& children)
    : children_()
    , indexOfCurChild(children.size())
{
    checkMetadata(children);

    children_ = std::move(children);
}

std::optional<std::vector<std::any>> Sequencer::processNext()
{
    if (!hasNext()) {
        return std::nullopt;
    }

    auto output = children_[indexOfCurChild]->processNext();
    while (output == std::nullopt) {
        ++indexOfCurChild;
        if (indexOfCurChild == children_.size()) {
            return std::nullopt;
        }
        
        children_[indexOfCurChild]->open();
        output = children_[indexOfCurChild]->processNext();
    }

    return output;
}

}// namespace codein
