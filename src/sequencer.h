/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <fstream>
#include <vector>

#include "expression.h"
#include "iterator.h"

#pragma once

namespace codein {

/**
 * @brief Sequence iterator to read from multiple children sequentially that share common output data format. 
 */
class Sequencer : public Iterator {
public:
    template <typename T, typename... ArgTs>
    friend std::unique_ptr<Iterator> makeIterator(ArgTs&&...);

    void open()
    {
        children_[0]->open();
        indexOfCurChild = 0;
    }

    void reopen() override 
    {
        open();
    }

    bool hasNext() const override 
    {
        return indexOfCurChild < children_.size();
    }

    std::optional<std::vector<std::any>> processNext() override; 

    void close() override
    {
        children_[indexOfCurChild]->close();
        indexOfCurChild = children_.size();
    }

    const Metadata& getMetadata() const override
    {
        return children_[0]->getMetadata();
    }

    ~Sequencer() override {}

private:
    /**
     * @brief Constructs a new Sequencer object
     * 
     * @param children: vector of children iterator.
     * Invariant: Children iterators must have same metadata in order to have equal output data format. 
     * Otherwise, Sequencer will throw an exception.
     */
    Sequencer(std::vector<std::unique_ptr<Iterator>>&& children);

    void checkMetadata(const std::vector<std::unique_ptr<Iterator>>& children);

    // sequence of children iterators.
    std::vector<std::unique_ptr<Iterator>> children_;
    // index variable to keep track of which file is being read.
    size_t indexOfCurChild;
};

/**
 * @brief Exception for when children iterators have different metadata, 
 * resulting in different output data format.
 */
class DiscrepantOutputData {};

} // namespace codein
