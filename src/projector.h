/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <any>
#include <memory>
#include <optional>
#include <vector>
#include <utility>

#include "iterator.h"
#include "expression.h"

#pragma once

namespace codein {

/**
 * @brief Projection iterator. Selects columns from input columns.
 */
class Projector : public Iterator {
public:
    template <typename T, typename... ArgTs>
    friend std::unique_ptr<Iterator> makeIterator(ArgTs&&...);

    /// Just open the child.
    void open() override
    {
        child_->open();
    }

    void reopen() override
    {
        child_->reopen();
    }

    bool hasNext() const override
    {
        return child_->hasNext();
    }

    /**
     * @brief 
     * 
     * @return std::optional<std::vector<std::any>> 
     */
    std::optional<std::vector<std::any>> processNext() override;

    void close() override
    {
        child_->close();
    }

    const Metadata& getMetadata() const override
    {
        return outputMetadata_;
    }

    ~Projector() override
    {}

private:
    /**
     * @brief Construct a new Projector object
     * 
     * @param child 
     * @param projections 
     * @param metadata 
     */
    Projector(std::unique_ptr<Iterator>&& child, const std::vector<Expression>& projections, Metadata metadata);

    std::unique_ptr<Iterator> child_;
    const Metadata& inputMetadata_;
    std::vector<Expression> projections_;
    Metadata outputMetadata_;
};

} // namespace codein
